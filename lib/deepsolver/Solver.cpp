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

//Uncomment this if you'd like to get full tracing in log;
//#define UNBOUNDED_LOG

DEEPSOLVER_BEGIN_SOLVER_NAMESPACE

class Bfs
{
public:
  Bfs(RefCountedEntries& p)
    : m_p(p) {}

      virtual ~Bfs() {}

  public:
  void go(const VarIdSet startSet1, const VarIdSet& startSet2)
  {
    VarIdVector startFrom;
    for(VarIdSet::const_iterator it = startSet1.begin();it != startSet1.end();++it)
      startFrom.push_back(*it);
    for(VarIdSet::const_iterator it = startSet2.begin();it != startSet2.end();++it)
      startFrom.push_back(*it);
    go(startFrom);
  }

      void go(const VarIdVector& startFrom)
      {
	const clock_t start = clock();
	assert(!startFrom.empty());
	for(VarIdVector::size_type i = 0;i < startFrom.size();++i)
	  {
	    assert(!m_p.getEntry(startFrom[i]).ambiguous);
	  m_pending.push_back(startFrom[i]);
	  }
	while(!m_pending.empty())
	  {
	    const VarId varId = m_pending.back();
	    m_pending.pop_back();
	    assert(m_p.hasEntry(varId));
	    RefCountedEntry& e = m_p.getEntry(varId);
	    assert(!e.ambiguous);
	    if (e.bfsMark)
	      continue;
	    e.bfsMark = 1;
	    for(Sat::size_type s = 0;s < e.sat.size();++s)
	      for(Clause::size_type c = 0;c < e.sat[s].size();++c)
		{
		  const VarId v = e.sat[s][c].varId;
		  const bool ambiguous = e.sat[s][c].ambiguous;
		  if (!ambiguous && v != e.varId && 
		      !m_p.getEntry(v).ambiguous)
		    m_pending.push_back(v);
		  }
	  }
	const double duration = (double)(clock() - start) / CLOCKS_PER_SEC;
	logMsg(LOG_DEBUG, "solver:BFS completed in %f sec", duration);
      }

  private:
      RefCountedEntries& m_p;
      VarIdVector m_pending;
}; //class Bfs;

void SatBuilder::build(const UserTask& userTask)
{
  const clock_t start = clock();
  p.clear();
  p.init(m_scope.getPkgCount());
  m_userTaskInstall.clear();
  m_userTaskRemove.clear();
  m_pending.clear();
#ifdef DEEPSOLVER_SOLVER_DEBUG
  m_debugReferences.resize(m_scope.getPkgCount());
  for(VarIdVector::size_type i = 0;i < m_debugReferences.size();++i)
    m_debugReferences[i] = BadVarId;
#endif //DEEPSOLVER_SOLVER_DEBUG;
  onUserTask(userTask);
  //FIXME:Check user task collision;
  for(VarIdSet::const_iterator it = m_userTaskInstall.begin();it != m_userTaskInstall.end();++it)
    {
      if (m_scope.isInstalled(*it))
	{
	  logMsg(LOG_DEBUG, "solver:user:%s already installed, no need to do it again", m_scope.getDesignation(*it).c_str());
	  continue;
	}
      logMsg(LOG_DEBUG, "solver:user:install %s", m_scope.getDesignation(*it).c_str());
      toBeInstalled(*it);
    }
  for(VarIdSet::const_iterator it = m_userTaskRemove.begin();it != m_userTaskRemove.end();++it)
    {
      if (!m_scope.isInstalled(*it))
	{
	  logMsg(LOG_DEBUG, "solver:user:%s is uninstalled, no need to remove it", m_scope.getDesignation(*it).c_str());
	  continue;
	}
      logMsg(LOG_DEBUG, "solver:user:remove %s", m_scope.getDesignation(*it).c_str());
      toBeRemoved(*it);
    }
  logMsg(LOG_DEBUG, "solver:user task yields %zu pending packages", m_pending.size());
  onPending();
  const double duration = (double)(clock() - start) / CLOCKS_PER_SEC;
    logMsg(LOG_DEBUG, "solver:SAT construction completed in %f sec", duration);
  size_t installedGermCount = 0, uninstalledGermCount = 0;
  size_t installedCount = 0, uninstalledCount = 0;
  for(VarId i = 0;i < p.size();++i)
    if (p.hasEntry(i))
      {
	const RefCountedEntry& e = p.getEntry(i);
	if (e.germ)
	  {
	    if (e.oldState)
	      installedGermCount++; else 
	      uninstalledGermCount++;
	  } else
	  {
	    if (e.oldState)
	      installedCount++; else
	      uninstalledCount++;
	  }
      }
  logMsg(LOG_DEBUG, "solver:%zu installed packages involved", installedCount);
  logMsg(LOG_DEBUG, "solver:%zu uninstalled packages involved", uninstalledCount);
  logMsg(LOG_DEBUG, "solver:%zu installed germs involved", installedGermCount);
  logMsg(LOG_DEBUG, "solver:%zu uninstalled germs involved", uninstalledGermCount);
  assert(ensureSatCorrect());
  p.clearGerms();
#ifdef DEEPSOLVER_SOLVER_DEBUG
  logMsg(LOG_DEBUG, "Dumping references to installed packages:");
  for(VarIdVector::size_type i = 0;i < m_debugReferences.size();++i)
    if (m_debugReferences[i] != BadVarId && m_scope.isInstalled(i))
      logMsg(LOG_DEBUG, "installed %s has initial reference from %s", m_scope.getPkgName(i).c_str(), m_scope.getDesignation(m_debugReferences[i]).c_str());
  logMsg(LOG_DEBUG, "Dumping references to uninstalled packages:");
  for(VarIdVector::size_type i = 0;i < m_debugReferences.size();++i)
    if (m_debugReferences[i] != BadVarId && !m_scope.isInstalled(i))
      logMsg(LOG_DEBUG, "%s has initial reference from %s", m_scope.getDesignation(i).c_str(), m_scope.getDesignation(m_debugReferences[i]).c_str());
#endif //DEEPSOLVER_SOLVER_DEBUG;
}

bool SatBuilder::ensureSatCorrect() const
{
  size_t clauseNum = 0, litNum = 0;
  for(VarId i = 0;i < p.size();++i)
    if(p.hasEntry(i))
      {
	const RefCountedEntry& e = p.getEntry(i);
	for(Sat::size_type s = 0;s < e.sat.size();++s)
	  {
	    litNum += e.sat[s].size();
	    for(Clause::size_type c = 0;c < e.sat[s].size();++c)
	      {
	      if (!p.hasEntry(e.sat[s][c].varId))
		{
		  logMsg(LOG_ERR, "%s contains %s literal without entry %s", m_scope.getDesignation(i).c_str(),
			 (e.sat[s][c].neg?"negative":"positive"),
			 m_scope.getDesignation(e.sat[s][c].varId).c_str());
		  return 0;
		}

	      if (p.getEntry(e.sat[s][c].varId).germ)
		{
		  logMsg(LOG_ERR, "%s contains %s literal with germ entry %s", m_scope.getDesignation(i).c_str(), 
			 (e.sat[s][c].neg?"negative":"positive"),
			 m_scope.getDesignation(e.sat[s][c].varId).c_str());
		  return 0;
		}


	      } //for(literals); 
	  } //for(clauses);
	clauseNum += e.sat.size();
      } //if(hasEntry());
  logMsg(LOG_DEBUG, "solver:SAT check completed successfully: %zu clauses with %zu literals", clauseNum, litNum);
  return 1;
}

void SatBuilder::onPending()
{
  while(!m_pending.empty())
    {
      const VarId varId = m_pending.back();
      m_pending.pop_back();
      if (p.hasEntry(varId) && !p.getEntry(varId).germ)
	continue;
      if (!m_scope.isInstalled(varId))
	toBeInstalled(varId); else
	toBeRemoved(varId);
    }
}

void SatBuilder::reconstruct(const VarIdVector& vars)
{
  for(VarIdVector::size_type i = 0;i < vars.size();++i)
    {
      assert(p.hasEntry(vars[i]) && !p.getEntry(vars[i]).germ);
      if (!m_scope.isInstalled(vars[i]))
	toBeInstalled(vars[i]); else
	toBeRemoved(vars[i]);
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

void SatBuilder::use(VarId varId, VarId referenceFrom)
{
  assert(varId != BadVarId && referenceFrom != BadVarId);
#ifdef DEEPSOLVER_SOLVER_DEBUG
  if (varId < m_debugReferences.size() && m_debugReferences[varId] == BadVarId)
    m_debugReferences[varId] = referenceFrom;
#endif //DEEPSOLVER_SOLVER_DEBUG;
  m_pending.push_back(varId);
}

bool SatBuilder::useGermInstalled(VarId varId, VarId reconstruct)
{
  assert(varId != BadVarId && reconstruct != BadVarId);
  for(VarIdVector::size_type i = 0;i < m_pending.size();++i)
    if (m_pending[i] == varId)
      return 0;
  if (p.hasEntry(varId))
    {
      RefCountedEntry& e = p.getEntry(varId);
      assert(e.oldState && e.newState);
      if (!e.germ)
	return 0;
      for(VarIdVector::size_type i = 0;i < e.reconstruct.size();++i)
	if (e.reconstruct[i] == reconstruct)
	  return 1;
      e.reconstruct.push_back(reconstruct);
      return 1;
    }
  //  logMsg(LOG_DEBUG, "Creating germ for %s", m_scope.getDesignation(varId).c_str());
  RefCountedEntry& e = p.newEntry(varId, 1);//1 means the package is currently installed;
  e.germ = 1;
  e.reconstruct.push_back(reconstruct);
  return 1;
}

bool SatBuilder::useGermUninstalled(VarId varId, VarId reconstruct)
{
  assert(varId != BadVarId && reconstruct != BadVarId);
  for(VarIdVector::size_type i = 0;i < m_pending.size();++i)
    if (m_pending[i] == varId)
      return 0;
  if (p.hasEntry(varId))
    {
      RefCountedEntry& e = p.getEntry(varId);
      assert(!e.oldState && !e.newState);
      if (!e.germ)
	return 0;
      for(VarIdVector::size_type i = 0;i < e.reconstruct.size();++i)
	if (e.reconstruct[i] == reconstruct)
	  return 1;
      e.reconstruct.push_back(reconstruct);
      return 1;
    }
  RefCountedEntry& e = p.newEntry(varId, 0);//0 means the package is currently uninstalled;
  e.germ = 1;
  e.reconstruct.push_back(reconstruct);
  return 1;
}

void SatBuilder::toBeInstalled(VarId varId)
{
  toBeInstalled(varId, BadVarId);
}

void SatBuilder::toBeInstalled(VarId varId, VarId replacementFor)
{
  assert(varId != BadVarId && !m_scope.isInstalled(varId));
#ifdef UNBOUNDED_LOG
    logMsg(LOG_DEBUG, "to install %s", m_scope.getDesignation(varId).c_str());
#endif //UNBOUNDED_LOG;
  if (m_userTaskRemove.find(varId) != m_userTaskRemove.end())
      return;
  RefCountedEntry& entry = p.ensureEntryExists(varId, 0);//0 means currently not installed;
  assert(!entry.oldState && !entry.newState);
  if (entry.germ)
    {
      entry.germ = 0;
      reconstruct(entry.reconstruct);
      entry.reconstruct.clear();
    }
  entry.sat.clear();
  //Blocking other versions of this package (FIXME:Not every package requires that);
  VarIdVector otherVer;
  m_scope.selectMatchingVarsRealNames(m_scope.pkgIdOfVarId(varId), otherVer);
  noDoubling(otherVer);
  for(VarIdVector::size_type i = 0;i < otherVer.size();++i)
    if (otherVer[i] != varId)
      {
	if (!m_scope.isInstalled(otherVer[i]))
	  {
	    if (!useGermUninstalled(otherVer[i], varId))
	    entry.onConflict(otherVer[i]);
	  } else
	  {
	    use(otherVer[i], varId);
	    entry.onConflict(otherVer[i]);
	  }
      } //Blocking other versions;
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
      if (alternatives.empty())
	{
	  logMsg(LOG_ERR, "unmet %s in package %s", m_scope.getDesignation(requires[i]).c_str(), m_scope.getDesignation(varId).c_str());
      assert(0);//FIXME:Exception;
	}
      noDoubling(alternatives);
#ifdef UNBOUNDED_LOG
      logMsg(LOG_DEBUG, "require %s", m_scope.getDesignation(requires[i]).c_str());
      for(VarIdVector::size_type k = 0;k < alternatives.size();++k)
      	logMsg(LOG_DEBUG, "matching %s", m_scope.getDesignation(alternatives[k]).c_str());
#endif //UNBOUNDEDLOG;
      bool noGerms = 1;
      for(VarIdVector::size_type k = 0;k < alternatives.size();++k)
	{
	  if (!m_scope.isInstalled(alternatives[k]))
	    continue;
	  if (useGermInstalled(alternatives[k], varId))
	    noGerms = 0;
	}
      if (!noGerms)
	continue;
#ifdef UNBOUNDED_LOG
      logMsg(LOG_DEBUG, "no germs found");
#endif //UNBOUNDED_LOG;
	for(VarIdVector::size_type k = 0;k < alternatives.size();++k)
	  use(alternatives[k], varId);
      entry.onRequire(alternatives);
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
	if (!m_scope.isInstalled(vars[k]))
	  {
	    if (!useGermUninstalled(vars[k], varId))
	    entry.onConflict(vars[k]);
	  } else
	  {
	    use(vars[k], varId);
	    entry.onConflict(vars[k]);
	  }
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
      use(vars[i], varId);
    }
}

void SatBuilder::toBeRemoved(VarId varId)
{
  assert(varId != BadVarId && m_scope.isInstalled(varId));
  if (m_userTaskInstall.find(varId) != m_userTaskInstall.end())
      return;
  RefCountedEntry& entry = p.ensureEntryExists(varId, 1);//1 means currently installed;
  assert(entry.oldState && entry.newState);
  if (entry.germ)
    {
      entry.germ = 0;
      reconstruct(entry.reconstruct);
      entry.reconstruct.clear();
    }
  entry.sat.clear();
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
      bool noGerms = 1;
      VarIdVector reduced;
      for(VarIdVector::size_type k = 0;k < alternatives.size();++k)
	{
	  if (!m_scope.isInstalled(alternatives[k]))
	    continue;
	  if (useGermInstalled(alternatives[k], varId))
	    noGerms = 0;
	}
      if (!noGerms)
	continue;
	for(VarIdVector::size_type k = 0;k < alternatives.size();++k)
	  use(alternatives[k], varId);
      entry.onDependent(dependent[i], alternatives);
      use(dependent[i], varId);
    }
  /*
  //What about a replacement with newer version?
  VarIdVector replacements;
  m_scope.selectMatchingVarsRealNames(m_scope.pkgIdOfVarId(varId), replacements);
  noDoubling(replacements);
  for(VarIdVector::size_type i = 0;i < replacements.size();++i)
    if (replacements[i] == varId)
      {
	replacements[i] = replacements.back();
	replacements.pop_back();
	break;
      }
  if (!replacements.empty())
    {
      m_scope.selectTheNewest(replacements);
      assert(!replacements.empty());
      toBeInstalled(replacements.front(), varId);
    }
  */
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
  VarIdVector fixedToInstall;
  solveSat(builder.p, builder.userTaskInstall(), builder.userTaskRemove(), fixedToInstall);
#ifdef DEEPSOLVER_SOLVER_DEBUG
  size_t remainInstalledCount = 0;
  logMsg(LOG_DEBUG, "The following involved packages will stay in the system according by clear minisat solution:");
  for(VarId i = 0;i < builder.p.size();++i)
    if (builder.p.hasEntry(i))
    {
      const RefCountedEntry& e = builder.p.getEntry(i);
      if (!e.oldState || !e.newState)
	continue;
      logMsg(LOG_DEBUG, "%s", m_scope.getPkgName(e.varId).c_str());
      remainInstalledCount++;
    }
  logMsg(LOG_DEBUG, "%zu remain installed", remainInstalledCount);
#endif //DEEPSOLVER_SOLVER_DEBUG;
  filterSolution(builder.p, builder.userTaskInstall(), builder.userTaskRemove(), fixedToInstall);
#ifdef DEEPSOLVER_SOLVER_DEBUG
  logMsg(LOG_DEBUG, "The following references to packages being removed cannot be optimized: (the list contains only valuable entries)");
  for(VarId i = 0;i < builder.p.size();++i)
    if (builder.p.hasEntry(i))
      {
	const RefCountedEntry& e = builder.p.getEntry(i);
	if (e.ambiguous)
	  continue;
	for(Sat::size_type s = 0;s < e.sat.size();++s)
	  for(Clause::size_type c = 0;c < e.sat[s].size();++c)
	    if (e.sat[s][c].varId != e.varId &&
		m_scope.pkgIdOfVarId(e.sat[s][c].varId) != m_scope.pkgIdOfVarId(e.varId) &&
!e.sat[s][c].ambiguous &&
		!builder.p.getEntry(e.sat[s][c].varId).ambiguous &&
		!builder.p.getEntry(e.sat[s][c].varId).newState)
	    logMsg(LOG_DEBUG, "%s has reference from %s", m_scope.getDesignation(e.sat[s][c].varId).c_str(), m_scope.getDesignation(e.varId).c_str());
      }
#endif //DEEPSOLVER_SOLVER_DEBUG;


  /*
  for(size_t i = 0;i < builder.p.size();++i)
    if (builder.p.hasEntry(i))
      {
	const RefCountedEntry& e = builder.p.getEntry(i);
	if (!e.newState && e.replacementFor != BadVarId &&
	    builder.p.getEntry(e.replacementFor).oldState &&
	    !builder.p.getEntry(e.replacementFor).newState)
	  fixedToInstall.push_back(e.varId);
      }
  logMsg(LOG_DEBUG, "solver:%zu gathered as fixed to install", fixedToInstall.size());
  solveSat(builder.p, builder.userTaskInstall(), builder.userTaskRemove(), fixedToInstall);
  filterSolution(builder.p, builder.userTaskInstall(), builder.userTaskRemove(), fixedToInstall);
  */

  for(size_t i = 0;i < builder.p.size();++i)
    {
      if (!builder.p.hasEntry(i))
	continue;
      const RefCountedEntry& e = builder.p.getEntry(i);
      if (e.ambiguous)
	continue;
      if (!e.oldState && e.newState)
	install.push_back(i);
      if (e.oldState && !e.newState)
	remove.push_back(i);
    }
}

void Solver::dumpSat(const UserTask& userTask, std::ostream& s) const
{
  SatBuilder builder(m_taskSolverData.backend, m_taskSolverData.scope, m_taskSolverData.providePriority);
  builder.build(userTask);
  VarIdVector fixedToInstall;
  solveSat(builder.p, builder.userTaskInstall(), builder.userTaskRemove(), fixedToInstall);
  filterSolution(builder.p, builder.userTaskInstall(), builder.userTaskRemove(), fixedToInstall);

  for(size_t i = 0;i < builder.p.size();++i)
    if (builder.p.hasEntry(i) && !builder.p.getEntry(i).germ)
      {
	const RefCountedEntry& e = builder.p.getEntry(i);

	s << "# " << m_scope.getDesignation(e.varId) << std::endl;
	s << "# Installation: " << (e.oldState?"Yes":"No") << " -> " << (e.newState?"Yes":"No") << std::endl;
	s << "# Ambiguous: " << (e.ambiguous?"yes":"no") << std::endl;
	for(Sat::size_type ss = 0;ss < e.sat.size();++ss)
	  {
	    s << "(" << std::endl;
	    for(Clause::size_type c = 0;c < e.sat[ss].size();++c)
	      {
		const VarId varId = e.sat[ss][c].varId;
		const bool neg = e.sat[ss][c].neg;
		const bool ambiguous = e.sat[ss][c].ambiguous;
		if (e.ambiguous || ambiguous)
		  s << "#"; else
		  s << " ";
		if (neg)
		  s << " !"; else
		  s << "  ";
		s << m_scope.getDesignation(varId) << std::endl;
	      }
	    s << ")" << std::endl;
	  }
	s << std::endl;
      }
}

bool Solver::solveSat(RefCountedEntries& p,
			   const VarIdSet& userTaskInstall,
		      const VarIdSet& userTaskRemove,
		      const VarIdVector& fixedToInstall) const
{
  AbstractSatSolver::Ptr satSolver = createDefaultSatSolver();
  for(VarIdSet::const_iterator it = userTaskInstall.begin();it != userTaskInstall.end();++it)
    satSolver->addClause(unitClause(Lit(*it)));
  for(VarIdVector::size_type i = 0;i < fixedToInstall.size();++i)
    satSolver->addClause(unitClause(Lit(fixedToInstall[i])));
  for(VarIdSet::const_iterator it = userTaskRemove.begin();it != userTaskRemove.end();++it)
    satSolver->addClause(unitClause(Lit(*it, 1)));
  for(VarId i = 0;i < p.size();++i)
    if (p.hasEntry(i))
      for(Sat::size_type k = 0;k < p.getEntry(i).sat.size();++k)
	satSolver->addClause(p.getEntry(i).sat[k]);
  VarIdToBoolMap res;
  VarIdVector conflicts;//FIXME:Remove conflicts;
  if (!satSolver->solve(res, conflicts))
    return 0;
  for(VarIdToBoolMap::const_iterator it = res.begin();it != res.end();++it)
    p.getEntry(it->first).newState = it->second;
  return 1;
}

  void Solver::filterSolution(RefCountedEntries& p,
			      const VarIdSet& userTaskInstall,
			      const VarIdSet& userTaskRemove,
			      const VarIdVector& fixedToInstall) const
{
  p.clearAllMarks();

  while(1)
    {
      logMsg(LOG_DEBUG, "solver:new filtering pass");
      size_t releasedReferenceCount = 0;
      for(VarId i = 0;i < p.size();++i)
	if (p.hasEntry(i))
	  {
	    RefCountedEntry& e = p.getEntry(i);
	    if (e.ambiguous)
	    continue;
	    if (e.oldState == e.newState)
	      {
	    e.ambiguous = 1;
	    for(Sat::size_type s = 0;s < e.sat.size();++s)
	      for(Clause::size_type c = 0;c < e.sat[s].size();++c)
		if (e.sat[s][c].varId != e.varId)
		  releasedReferenceCount++;
	    continue;
	      } //the package not changed;
		for(Sat::size_type s = 0;s < e.sat.size();++s)
		  {
		    Clause::size_type c;
		    for(c = 0;c < e.sat[s].size();++c)
		      if (!e.sat[s][c].neg &&
e.sat[s][c].varId != e.varId &&
			  p.getEntry(e.sat[s][c].varId).oldState &&
			  p.getEntry(e.sat[s][c].varId).newState)
			break;

		    if 	      (c >= e.sat[s].size())
		      {
		    for(c = 0;c < e.sat[s].size();++c)
		      if (!e.sat[s][c].neg &&
			  p.getEntry(e.sat[s][c].varId).newState &&
			  p.getEntry(e.sat[s][c].varId).newState)
			break;
		      }

		    if 	      (c < e.sat[s].size())
		      for(Clause::size_type c1 = 0;c1 < e.sat[s].size();++c1)
			if (c1 != c &&
e.sat[s][c1].varId != e.varId &&
			    !e.sat[s][c1].ambiguous)
			  {
			    e.sat[s][c1].ambiguous = 1;
			    releasedReferenceCount++;
			  }
		  } //for(clauses);
	  } //for(entries);
      logMsg(LOG_DEBUG, "solver:%zu references released", releasedReferenceCount);
      if (releasedReferenceCount == 0)
return;

      p.clearBfsMarks();
      VarIdVector bfsSeed;
      for(VarIdSet::const_iterator it = userTaskInstall.begin();it != userTaskInstall.end();++it)
	bfsSeed.push_back(*it);
      for(VarIdSet::const_iterator it = userTaskRemove.begin();it != userTaskRemove.end();++it)
	bfsSeed.push_back(*it);
      for(VarIdVector::size_type i = 0;i < fixedToInstall.size();++i)
	bfsSeed.push_back(fixedToInstall[i]);
      Bfs(p).go(bfsSeed);

      size_t rolledBackCount = 0;
      for(VarId i = 0;i < p.size();++i)
	if (p.hasEntry(i))
	{
	    RefCountedEntry& e = p.getEntry(i);
	    if (!e.bfsMark && e.oldState != e.newState)
	      {
		e.newState = e.oldState;
		rolledBackCount++;
	      }
	}
      logMsg(LOG_DEBUG, "solver:%zu packages have been rolled back", rolledBackCount);
      if (rolledBackCount == 0)
	return;
    }
}

DEEPSOLVER_END_SOLVER_NAMESPACE

Deepsolver::AbstractTaskSolver::Ptr Deepsolver::createTaskSolver(const Deepsolver::TaskSolverData& taskSolverData)
{
  return AbstractTaskSolver::Ptr(new Solver::Solver(taskSolverData));
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
