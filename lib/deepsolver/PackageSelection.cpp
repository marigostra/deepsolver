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
#include"deepsolver/PackageSelection.h"
#include"deepsolver/PackageNameSorting.h"

DEEPSOLVER_BEGIN_NAMESPACE

VarId PackageSelection::translateItemToInstall(const UserTaskItemToInstall& item) const
{
  //FIXME:Obsoletes;
  assert(!item.pkgName.empty());
  const bool hasVersion = item.verDir != VerNone;
  assert(!hasVersion || !item.version.empty());
  if (!m_scope.checkName(item.pkgName))
    throw TaskException(TaskException::UnknownPackageName, item.pkgName);
  const PackageId pkgId = m_scope.strToPackageId(item.pkgName);
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
      m_scope.selectMatchingVarsAmongProvides(pkgId, ver, vars);
    } else
    m_scope.selectMatchingVarsAmongProvides(pkgId, vars);
  if (vars.empty())//No appropriate packages at all;
    throw TaskException(TaskException::NoRequestedPackage, item.toString());
  if (hasVersion || m_scope.allProvidesHaveTheVersion(vars, pkgId))
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

VarId PackageSelection::satisfyRequire(const IdPkgRel& rel) const
{
  if (!rel.hasVer())
    return satisfyRequire(rel.pkgId);
  return satisfyRequire(rel.pkgId, rel.extractVersionCond());
}

VarId PackageSelection::satisfyRequire(PackageId pkgId) const
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
  /*
   * We cannot find anything just by real names, so 
   * now the time to select anything among presented provides records;
   */
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

VarId PackageSelection::satisfyRequire(PackageId pkgId, const VersionCond& version) const
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
  /*
   * We cannot find anything just by real names, so 
   * now the time to select anything among presented provides records;
   */
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

VarId PackageSelection::processProvidesPriorityList(const VarIdVector& vars, PackageId provideEntry) const
{
  assert(!vars.empty());
  const std::string provideName = m_scope.packageIdToStr(provideEntry);
  TaskSolverProvideInfoVector::size_type k = 0;
  while(k < m_providesPriority.size() && provideName != m_providesPriority[k].name)
    k++;
  if (k >= m_providesPriority.size())
    return BadVarId;
  const StringVector& providers = m_providesPriority[k].providers;
  for(StringVector::size_type i = 0;i < providers.size();i++)
    {
      if (!m_scope.checkName(providers[i]))
	continue;
      const PackageId providerId = m_scope.strToPackageId(providers[i]);
      assert(providerId != BadPkgId);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	if (providerId == m_scope.packageIdOfVarId(vars[k]))
	  return vars[k];
    }
  return BadVarId;
}

VarId PackageSelection::processProvidesPriorityBySorting(const VarIdVector& vars) const
{
  assert(!vars.empty());
  PackageNameSortItemVector items;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    items.push_back(PackageNameSortItem(&m_backEnd, vars[i], m_scope.constructPackageName(vars[i])));
  std::sort(items.begin(), items.end());
  assert(!items.empty());
  return items[items.size() - 1].varId;
}

DEEPSOLVER_END_NAMESPACE
