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

#include"deepsolver/deepsolver.h"
#include"deepsolver/Solver.h"

DEEPSOLVER_BEGIN_SOLVER_NAMESPACE

typedef Sat::AbstractSatSolver::VarIdToBoolMap VarIdToBoolMap;

void SatBuilder::build(const UserTask& userTask)
{
  m_userTaskInstall.clear();
  m_userTaskRemove.clear();
  onUserTask(userTask);
  for(VarIdSet::const_iterator it = m_userTaskRemove.begin();it != m_userTaskInstall.end();++it)
    {
      p.incRef(*it);
      fillEntry(*it);
    }
  for(VarIdSet::const_iterator it = m_userTaskRemove.begin();it != m_userTaskRemove.end();++it)
    {
      p.incRef(*it);
      fillEntry(*it);
    }
}

void SatBuilder::onUserTask(const UserTask& userTask)
{
  // To install;
  for(UserTaskItemToInstallVector::size_type i = 0;i < userTask.itemsToInstall.size();++i)
    {
      const VarId varId = onUserItemToInstall(userTask.itemsToInstall[i]);
      assert(varId != BadVarId);
      m_userTaskInstall.insert(varId);
      VarIdVector otherVer;
      m_scope.selectMatchingVarsRealNames(m_scope.pkgIdOfVarId(varId), otherVer);
      noDoubling(otherVer);
      for(VarIdVector::size_type k = 0;k < otherVer.size();++k)
	if (otherVer[k] != varId)
	  m_userTaskRemove.insert(otherVer[k]);
    } //For items to install;
  // To remove;
  for(StringSet::const_iterator it = userTask.namesToRemove.begin() ;it != userTask.namesToRemove.end();++it)
    {
      assert(!it->empty());
      if (!m_scope.knownPkgName(*it))
	{
	  logMsg(LOG_DEBUG, "solver:request contains unknown package \'%s\' to remove , skipping", it->c_str());
	  //	  m_output.notifyUnknownPackageToRemove(*it);
	  continue;
	}
      const PkgId pkgId = m_scope.strToPkgId(*it);
      assert(pkgId != BadPkgId);
      VarIdVector vars;
      m_scope.selectMatchingVarsRealNames(pkgId, vars);
      noDoubling(vars);
      for(VarIdVector::size_type k = 0;k < vars.size();++k)
	m_userTaskRemove.insert(vars[k]);
    }
}

void SatBuilder::fillEntry(VarId varId)
{
  assert(varId != BadVarId);
  if (m_scope.isInstalled(varId))
    toBeRemoved(varId); else
    toBeInstalled(varId);
}

void SatBuilder::toBeInstalled(VarId varId)
{
  assert(varId != BadVarId && !m_scope.isInstalled(varId));
  if (p.hasEntry(varId))
    return;
  RefCountedEntry& entry = p.newEntry(varId, 0);//0 means not installed now;
  //Blocking other versions of this package (FIXME:Not every package requires that);
  VarIdVector otherVer;
  m_scope.selectMatchingVarsRealNames(m_scope.pkgIdOfVarId(varId), otherVer);
  noDoubling(otherVer);
  for(VarIdVector::size_type i = 0;i < otherVer.size();++i)
    if (otherVer[i] != varId)
      {
	fillEntry(otherVer[i]);
	entry.onConflict(otherVer[i]);
      }
  //Requires;
  IdPkgRelVector requires;
  m_scope.getRequires(varId, requires);
  for(IdPkgRelVector::size_type i = 0;i < requires.size();++i)
    {
      IdPkgRelVector::size_type doubling;
      for(doubling = 0;doubling < i;++doubling)
	if (requires[doubling] == requires[i])
	  break;
      if (doubling < i)
	continue;
      VarIdVector alternatives;
      m_scope.selectMatchingVarsWithProvides(requires[i], alternatives);
      assert(!alternatives.empty());//FIXME:Exception;
      noDoubling(alternatives);
      entry.onRequire(alternatives);
      for(VarIdVector::size_type k = 0;k < alternatives.size();++k)
	fillEntry(alternatives[k]);
    }
  //Conflicts;
  IdPkgRelVector conflicts;
  m_scope.getConflicts(varId, conflicts);
  for(IdPkgRelVector::size_type i = 0;i < conflicts.size();++i)
    {
      IdPkgRelVector::size_type doubling;
      for(doubling = 0;doubling < i;++doubling)
	if (conflicts[doubling] == conflicts[i])
	  break;
      if (doubling < i)
	continue;
      VarIdVector vars;
      m_scope.selectMatchingVarsWithProvides(conflicts[i], vars);
      noDoubling(vars);
      for(VarIdVector::size_type k = 0;k < vars.size();++k)
	if (vars[k] != varId)//Package may not conflict with itself;
	  {
	    entry.onConflict(vars[k]);
	    fillEntry(vars[k]);
	  }
    }
  //Here we check are there any conflicts among the installed packages;
  VarIdVector vars;
  IdPkgRelVector rels;
  m_scope.whatConflictAmongInstalled(varId, vars, rels);
  for(VarIdVector::size_type i = 0;i < vars.size();++i)
    {
      assert(m_scope.isInstalled(vars[i]));
      entry.onConflict(vars[i]);
      fillEntry(vars[i]);
    }
}

void SatBuilder::toBeRemoved(VarId varId)
{
  assert(varId != BadVarId && m_scope.isInstalled(varId));
  if (p.hasEntry(varId))
    return;
  RefCountedEntry& entry = p.newEntry(varId, 1);//1 means the package is installed now;
  VarIdVector dependent;
  IdPkgRelVector rels;
  m_scope.whatDependAmongInstalled(varId, dependent, rels);
  assert(dependent.size() == rels.size());
  for(VarIdVector::size_type i = 0;i < dependent.size();++i)
    {
      VarIdVector alternatives;
      m_scope.selectMatchingVarsWithProvides(rels[i], alternatives);
      noDoubling(alternatives);
      for(VarIdVector::size_type k = 0;k < alternatives.size();++k)
	if (alternatives[k] == varId)
	  {
	    alternatives[k] = alternatives.back();
	    alternatives.pop_back();
	    break;
	  }
      entry.onDependent(dependent[i], alternatives);
    }
}

VarId SatBuilder::onUserItemToInstall(const UserTaskItemToInstall& item) const
{
  //FIXME:Obsoletes;
  assert(!item.pkgName.empty());
  const bool hasVersion = item.verDir != VerNone;
  assert(!hasVersion || !item.version.empty());
  if (!m_scope.knownPkgName(item.pkgName))
    throw TaskException(TaskException::UnknownPackageName, item.pkgName);
  const PkgId pkgId = m_scope.strToPkgId(item.pkgName);
  assert(pkgId != BadPkgId);
  VarIdVector vars;
  if (!hasVersion)
    {
      //The following line does not take into account available provides;
      m_scope.selectMatchingVarsRealNames(pkgId, vars);
    } else
    {
      VersionCond ver(item.version, item.verDir);
      //This line does not handle provides too;
      m_scope.selectMatchingVarsRealNames(pkgId, ver, vars);
    }
  if (!vars.empty())
    {
      m_scope.selectTheNewest(vars);
      assert(!vars.empty());
      //We can get here more than one the newest packages, assuming no difference what exact one to take;
      return vars.front();
    }
  /*
   * We cannot find anything just by real names (no package with required
   * name at all or there have inappropriate version restrictions ), so 
   * now the time to select anything among presented provides records;
   */
  if (hasVersion)
    {
      VersionCond ver(item.version, item.verDir);
      m_scope.selectMatchingVarsProvidesOnly(pkgId, ver, vars);
    } else
    m_scope.selectMatchingVarsProvidesOnly(pkgId, vars);
  if (vars.empty())//No appropriate packages at all;
    throw TaskException(TaskException::NoRequestedPackage, item.toString());
  if (hasVersion || m_scope.allProvidesHaveTheVersion(vars, pkgId))
    {
      const VarId res = m_providePriority.byProvidesPriorityList(vars, pkgId);
      if (res != BadVarId)
	return res;
      m_scope.selectTheNewestByProvide(vars, pkgId);
      assert(!vars.empty());
      if (vars.size() == 1)
	return vars.front();
      return byProvidesPrioritySorting(vars);
    }
  const VarId res = m_providePriority.byProvidesPriorityList(vars, pkgId);
  if (res != BadVarId)
    return res;
  return byProvidesPrioritySorting(vars);
}

VarId SatBuilder::byProvidesPrioritySorting(const VarIdVector& vars) const
{
  assert(!vars.empty());
  VarIdVector v(vars);
  std::sort(v.begin(), v.end(), [this] (VarId var1, VarId var2) {
    const std::string name1 = m_scope.getPkgName(var1);
    const std::string name2 = m_scope.getPkgName(var2);
    if (name1 == name2)
      return 0;
    return m_backend.verCmp(name1, name2);
  });
  return v.back();
}

void Solver::solve(const UserTask& userTask, 
		   VarIdVector& install,
		   VarIdVector& remove) const
{
  SatBuilder builder(m_taskSolverData.backend, m_taskSolverData.scope, m_taskSolverData.providePriority);
  builder.build(userTask);
  solveSat(builder.p, builder.userTaskInstall(), builder.userTaskRemove());
  //FIXME:
}

bool Solver::solveSat(RefCountedEntries& p,
			   const VarIdSet& userTaskInstall,
			   const VarIdSet& userTaskRemove) const
{
  std::auto_ptr<Sat::AbstractSatSolver> satSolver = Sat::createDefaultSatSolver();
  for(VarIdSet::const_iterator it = userTaskInstall.begin();it != userTaskInstall.end();++it)
    satSolver->addClause(Sat::unitClause(Sat::Lit(*it)));
  for(VarIdSet::const_iterator it = userTaskRemove.begin();it != userTaskRemove.end();++it)
    satSolver->addClause(Sat::unitClause(Sat::Lit(*it, 1)));
  for(VarId i = 0;i < p.size();++i)
    if (p.hasEntry(i))
      for(Sat::Sat::size_type k = 0;k < p.getEntry(i).sat.size();++k)
	satSolver->addClause(p.getEntry(i).sat[k]);
  VarIdToBoolMap res;
  VarIdVector conflicts;//FIXME:Remove conflicts;
  if (!satSolver->solve(res, conflicts))
    return 0;
  for(VarIdToBoolMap::const_iterator it = res.begin();it != res.end();++it)
    p.getEntry(it->first).newState = it->second;
}

void Solver::filterSolution(RefCountedEntries& p) const
{
  bool cont = 1;
  while(cont)
    {
      for(VarId i = 0;i < p.size();++i)
	if (p.hasEntry(i))
	  {
	    const RefCountedEntry& e = p.getEntry(i);
	    if (e.oldState != e.newState)
	      continue;
	    for(Sat::Sat::size_type s = 0;s < e.sat.size();++s)
	      for(Sat::Clause::size_type c = 0;c < e.sat[s].size();++c)
		p.decRef(e.sat[s][c].varId);
	  }
      for(VarId i = 0;i < p.size();++i)
	if (p.hasEntry(i))
	{
	    RefCountedEntry& e = p.getEntry(i);
	    if (e.refNum == 0 && e.oldState != e.newState)
	      {
		e.newState = e.oldState;
		cont = 1;
	      }
	}
    }
}

DEEPSOLVER_END_SOLVER_NAMESPACE

std::auto_ptr<Deepsolver::AbstractTaskSolver> Deepsolver::createTaskSolver(const Deepsolver::TaskSolverData& taskSolverData)
{
  return std::auto_ptr<AbstractTaskSolver>(new Solver::Solver(taskSolverData));
}

  /*Potentially needed:
VarId satisfyRequire(const IdPkgRel& rel) const
{
  if (!rel.hasVer())
    return satisfyRequire(rel.pkgId);
  return satisfyRequire(rel.pkgId, rel.extractVersionCond());
}

VarId satisfyRequire(PackageId pkgId) const
{
  assert(pkgId != BadPkgId);
  VarIdVector vars;
  //The following line does not take into account available provides;
  m_scope.selectMatchingVarsRealNames(pkgId, vars);
  if (!vars.empty())
    {
      m_scope.selectTheNewest(vars);
      assert(!vars.empty());
      //We can get here more than one the newest packages, assuming no difference what exact one to take;
      return vars.front();
    }

   * We cannot find anything just by real names, so 
   * now the time to select anything among presented provides records;

  m_scope.selectMatchingVarsAmongProvides(pkgId, vars);
  if (vars.empty())//No appropriate packages at all;
    {
      logMsg(LOG_ERR, "No matching package for require entry \'%s\'", m_scope.packageIdToStr(pkgId).c_str());
      throw TaskException(TaskException::Unmet, m_scope.packageIdToStr(pkgId));
    }
  if (m_scope.allProvidesHaveTheVersion(vars, pkgId))
    {
      const VarId res = processProvidesPriorityList(vars, pkgId);
      if (res != BadVarId)
	return res;
      m_scope.selectTheNewestByProvide(vars, pkgId);
      assert(!vars.empty());
      if (vars.size() == 1)
	return vars.front();
      return processProvidesPriorityBySorting(vars);
    }
  const VarId res = processProvidesPriorityList(vars, pkgId);
  if (res != BadVarId)
    return res;
  return processProvidesPriorityBySorting(vars);
}

VarId satisfyRequire(PackageId pkgId, const VersionCond& version) const
{
  assert(version.type != VerNone);
  assert(!version.version.empty());
  assert(pkgId != BadPkgId);
  VarIdVector vars;
  //This line does not handle provides ;
  m_scope.selectMatchingVarsRealNames(pkgId, version, vars);
  if (!vars.empty())
    {
      m_scope.selectTheNewest(vars);
      assert(!vars.empty());
      //We can get here more than one the newest packages, assuming no difference what exact one to take;
      return vars.front();
    }

   * We cannot find anything just by real names, so 
   * now the time to select anything among presented provides records;

  m_scope.selectMatchingVarsAmongProvides(pkgId, version, vars);
  if (vars.empty())//No appropriate packages at all;
    {
      //FIXME:      logMsg(LOG_ERR, "No matching package for require entry \'%s\'", relToString(IdPkgRel(pkgId, version)).c_str());
      throw TaskException(TaskException::Unmet, m_scope.packageIdToStr(pkgId));
    }
  const VarId res = processProvidesPriorityList(vars, pkgId);
  if (res != BadVarId)
    return res;
  m_scope.selectTheNewestByProvide(vars, pkgId);
  assert(!vars.empty());
  if (vars.size() == 1)
    return vars.front();
  return processProvidesPriorityBySorting(vars);
}
*/
