/*
   Copyright 2011-2013 ALT Linux
   Copyright 2011-2013 Michael Pozhidaev

   This file is part of the Deepsolver.

   Deepsolver is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   Deepsolver is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
*/

#include"deepsolver.h"
#include"TaskTranslator.h"

void TaskTranslator::translate(const UserTask& task)
{
  m_pending.clear();
  m_processed.clear();
  translateUserTask(task);
  processPendings();
}

void TaskTranslator::translateUserTask(const UserTask& userTask)
{
  // To install;
  for(UserTaskItemToInstallVector::size_type i = 0;i < userTask.itemsToInstall.size();i++)
    {
      //The following line checks the entire package scope including installed packages;
      const VarId varId = m_selection.translateItemToInstall(userTask.itemsToInstall[i]);
      assert(varId != BAD_VAR_ID);
      m_pending.push_back(varId);
      m_output.onUserTaskInstall(varId);
      VarIdVector otherVersions;
      m_scope.selectMatchingVarsRealNames(m_scope.packageIdOfVarId(varId), otherVersions);
      rmDub(otherVersions);
      for(VarIdVector::size_type k = 0;k < otherVersions.size();k++)
	if (otherVersions[k] != varId)
	  {
	    m_pending.push_back(otherVersions[k]);//FIXME:Not every package require this;
	    m_output.onBlockingByOtherVersion(otherVersions[k], varId);
	  }
    } //For items to install;
  // To remove;
  for(StringSet::const_iterator it = userTask.namesToRemove.begin() ;it != userTask.namesToRemove.end();it++)
    {
      assert(!it->empty());
      if (!m_scope.checkName(*it))
	{
	  logMsg(LOG_DEBUG, "translator:request contains ask to remove unknown package \'%s\', skipping", it->c_str());
	  m_output.notifyUnknownPackageToRemove(*it);
	  continue;
	}
      const PackageId pkgId = m_scope.strToPackageId(*it);
      assert(pkgId != BAD_PACKAGE_ID);
      VarIdVector vars;
      m_scope.selectMatchingVarsRealNames(pkgId, vars);
      rmDub(vars);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	{
	  m_pending.push_back(vars[k]);
	  m_output.onUserTaskRemove(vars[k]);
	}
    }
}

void TaskTranslator::handleChangeToTrue(VarId varId)
{
  assert(varId != BAD_VAR_ID);
  assert(!m_scope.isInstalled(varId));
  //Blocking other versions of this package;
  //FIXME:Not every package requires this;
  VarIdVector otherVersions;
  m_scope.selectMatchingVarsRealNames(m_scope.packageIdOfVarId(varId), otherVersions);
  rmDub(otherVersions);
  for(VarIdVector::size_type i = 0;i < otherVersions.size();i++)
    if (otherVersions[i] != varId)
      {
	m_output.onlyOneMayBeInstalled(varId, otherVersions[i]);
	m_pending.push_back(otherVersions[i]);
      }
  //Requires;
  IdPkgRelVector requires;
  m_scope.getRequires(varId, requires);
  for(IdPkgRelVector::size_type i = 0;i < requires.size();i++)
    {
      IdPkgRelVector::size_type dub;
      for(dub = 0;dub < i;dub++)
	if (requires[dub] == requires[i])
	  break;
      if (dub < i)
	continue;
      VarIdVector alternatives;
      m_scope.selectMatchingVarsWithProvides(requires[i], alternatives);
      rmDub(alternatives);
      VarIdVector installed;
      bool haveOneBeInstalledAnyway = 0;
      for(VarIdVector::size_type k = 0;k < alternatives.size();k++)
	{
	  if (m_processed.find(alternatives[k]) == m_processed.end() && m_scope.isInstalled(alternatives[k]))
	  installed.push_back(alternatives[k]);
	if (m_output.wasConsideredInstallAnyway(alternatives[k]))
	  haveOneBeInstalledAnyway = 1;
	}
      /*
       * We may forget about this require at all since at least one alternative
       * will be installed anyway. 
       */
      if (haveOneBeInstalledAnyway)
	continue;
      if (installed.empty())
	{
	  m_output.requiredOneAnyForPackage(varId, alternatives);
	  for(VarIdVector::size_type k = 0;k < alternatives.size();k++)
	    m_pending.push_back(alternatives[k]);
	} else
	m_postponedRequires.takeNewRequire(varId, alternatives, installed);
	}
  //Conflicts;
  IdPkgRelVector conflicts;
  m_scope.getConflicts(varId, conflicts);
  for(IdPkgRelVector::size_type i = 0;i < conflicts.size();i++)
    {
      IdPkgRelVector::size_type dub;
      for(dub = 0;dub < i;dub++)
	if (conflicts[dub] == conflicts[i])
	  break;
      if (dub < i)
	continue;
      VarIdVector vars;
      m_scope.selectMatchingVarsWithProvides(conflicts[i], vars);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	if (vars[k] != varId)//Package may not conflict with itself;
	  {
	    m_output.onlyOneMayBeInstalled(varId, vars[k]);
	    m_pending.push_back(vars[k]);
	  }
    }

  //Here we check are there any conflicts from the installed packages;
  VarIdVector vars;
  IdPkgRelVector rels;
  m_scope.whatConflictsAmongInstalled(varId, vars, rels);
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(m_scope.isInstalled(vars[i]));
      m_output.onlyOneMayBeInstalled(varId, vars[i]);
      m_pending.push_back(vars[i]);
    }
}

void TaskTranslator::handleChangeToFalse(VarId varId)
{
  assert(varId != BAD_VAR_ID);
  assert(m_scope.isInstalled(varId));
  m_postponedRequires.varIdAffected(varId, m_output, m_pending);
  processVersionUpgrade(varId);
  VarIdVector deps;
  IdPkgRelVector rels;
  m_scope.whatDependsAmongInstalled(varId, deps, rels);
  assert(deps.size() == rels.size());
  for(VarIdVector::size_type i = 0;i < deps.size();i++)
    {
      VarIdVector alternatives;
      m_scope.selectMatchingVarsWithProvides(rels[i], alternatives);
      rmDub(alternatives);
      VarIdVector installed;
      bool haveOneBeInstalledAnyway = 0;
      for(VarIdVector::size_type k = 0;k < alternatives.size();k++)
	{
	  if (varId != alternatives[k] && m_processed.find(alternatives[k]) == m_processed.end() && m_scope.isInstalled(alternatives[k]))
	    installed.push_back(alternatives[k]);
	if (m_output.wasConsideredInstallAnyway(alternatives[k]))
	  haveOneBeInstalledAnyway = 1;
	}
      if (haveOneBeInstalledAnyway)
	continue;
	        if (installed.empty())
	{
	  m_pending.push_back(deps[i]);
	  m_output.requiredOneAnyForPackage(deps[i], alternatives);
	  for(VarIdVector::size_type k = 0;k < alternatives.size();k++)
	    m_pending.push_back(alternatives[k]);
	} else
	m_postponedRequires.takeNewRequire(varId, alternatives, installed);
    }
}

void TaskTranslator::processVersionUpgrade(VarId varId)
{
  assert(varId != BAD_VAR_ID);
  const PackageId pkgId = m_scope.packageIdOfVarId(varId);
  VarIdVector otherVersions;
  m_scope.selectMatchingVarsRealNames(pkgId, otherVersions);
  rmDub(otherVersions);
  m_scope.selectTheNewest(otherVersions);
  if (otherVersions.empty())
    return;
  const VarId replacement = otherVersions[0];
  if (replacement == varId ||
      m_output.wasConsideredInstallAnyway(replacement) ||
      m_output.wasConsideredRemoveAnyway(replacement))
    return;
  IdPkgRelVector requires;
  m_scope.getRequires(replacement, requires);
  for(IdPkgRelVector::size_type i = 0;i < requires.size();i++)
    {
      VarIdVector alternatives;
      m_scope.selectMatchingVarsWithProvides(requires[i], alternatives);
      rmDub(alternatives);
      VarIdVector::size_type k;
      for(k = 0;k < alternatives.size();k++)
	if ((m_scope.isInstalled(alternatives[k]) && !m_output.wasConsideredRemoveAnyway(alternatives[k])) || 
	    m_output.wasConsideredInstallAnyway(alternatives[k]))
	  break;
      if (k >= alternatives.size())
	return;
    }
  logMsg(LOG_DEBUG, "translator:%s may be a replacement for %s", m_scope.constructPackageName(replacement).c_str(), m_scope.constructPackageName(varId).c_str());
  m_output.onlyOneInstallAnyway(varId, replacement);
  m_pending.push_back(replacement);
}

void TaskTranslator::processPendings()
{
  /*
   * The important idea of this method is to process all available packages
   * to install before processing any package to remove because it allows
   * SAT optimizer collect more information which packages considered to
   * install anyway and prevent needless SAT equation extending.
   */
  while(!m_pending.empty())
    {
      for(VarIdVector::size_type i = 0;i < m_pending.size();i++)
	{
	  assert(m_pending[i] != BAD_VAR_ID);
	  //Checking only packages to install;
	  if (m_scope.isInstalled(m_pending[i]))
	    continue;
	  const VarId varId = m_pending[i];
	  m_pending[i] = BAD_VAR_ID;
	  if (m_processed.find(varId) != m_processed.end())
	    continue;
	  m_processed.insert(varId);
	  handleChangeToTrue(varId);
	}
      for(VarIdVector::size_type i = 0;i < m_pending.size();i++)
	while(i < m_pending.size() && m_pending[i] == BAD_VAR_ID)
	  {
	    m_pending[i] = m_pending.back();
	    m_pending.pop_back();
	  }
	  while(!m_pending.empty() && m_processed.find(m_pending.back()) != m_processed.end())
	    m_pending.pop_back();
      if (!m_pending.empty())
	{
	  assert(m_processed.find(m_pending.back()) == m_processed.end());
	  assert(m_scope.isInstalled(m_pending.back()));
	  m_processed.insert(m_pending.back());
	  handleChangeToFalse(m_pending.back());
	  m_pending.pop_back();
	}
    }
}

std::string TaskTranslator::relToString(const IdPkgRel& rel)
{
  const std::string ver = rel.verString();
  if (ver.empty())
      return m_scope.packageIdToStr(rel.pkgId);
  return m_scope.packageIdToStr(rel.pkgId) + " " + ver;
}
