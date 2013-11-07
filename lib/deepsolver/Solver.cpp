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
      p.incRef(*it);
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
      p.incRef(*it);
    }
  logMsg(LOG_DEBUG, "solver:user task yields %zu pending packages", m_pending.size());
  onPending();
  preSatOptimization();
  logMsg(LOG_DEBUG, "solver:got %zu replacements to add", m_replPending.size());
  for(VarIdToVarIdMap::const_iterator it = m_replPending.begin();it != m_replPending.end();++it)
    if (p.hasEntry(it->first))
    toBeInstalled(it->second, it->first);
  onPending();
  const double duration = (double)(clock() - start) / CLOCKS_PER_SEC;
    logMsg(LOG_DEBUG, "solver:SAT construction completed in %f sec", duration);
}

void SatBuilder::onPending()
{
  while(!m_pending.empty())
    {
      const VarId varId = m_pending.back();
      m_pending.pop_back();
      if (p.hasEntry(varId))
	{
	  p.incRef(varId);
	  continue;
	}
      if (!m_scope.isInstalled(varId))
	toBeInstalled(varId); else
	toBeRemoved(varId);
      p.incRef(varId);
    }

}

void SatBuilder::preSatOptimization()
{
  BoolVector rem;
  rem.resize(m_scope.getPkgCount());
  while(1)
    {
      logMsg(LOG_DEBUG, "solver:new pass for pre-sat optimization");
  for(VarId i = 0;i < rem.size();++i)
    rem[i] = m_scope.isInstalled(i);
  assert(rem.size() == p.size());
  //Collecting installed packages without negative literals;
  for(VarId i = 0;i < p.size();++i)
    if (p.hasEntry(i))
      {
	const RefCountedEntry& e = p.getEntry(i);
	for(Sat::size_type s = 0;s < e.sat.size();++s)
	  for(Clause::size_type c = 0;c < e.sat[s].size();++c)
	    if (e.sat[s][c].neg)
	      {
		assert(e.sat[s][c].varId < rem.size());
		rem[e.sat[s][c].varId] = 0;
	      }
      }    
  //Removing clauses containing obviously true literals;
  for(VarId i = 0;i < p.size();++i)
    if (p.hasEntry(i))
      for(Sat::size_type s = 0;s < p.getEntry(i).sat.size();++s)
	{
	  RefCountedEntry& e = p.getEntry(i);
	  Clause::size_type c;
	  for(c = 0;c < e.sat[s].size();++c)
	    {
	      assert(e.sat[s][c].varId < rem.size());
	      //TODO:Actually we are able to do something similar and with packages to remove;
	      if (!e.sat[s][c].neg &&
		  (rem[e.sat[s][c].varId] ||
		   m_userTaskInstall.find(e.sat[s][c].varId) != m_userTaskInstall.end()))
		break;
	    }
	  if (c >= e.sat[s].size())
	    continue;
	  assert(!e.sat[s][c].neg);
	  e.sat[s].clear();
	}
  size_t clausesRemoved = 0;
  for(VarId i = 0;i < p.size();++i)
    if (p.hasEntry(i))
      {
	RefCountedEntry& e = p.getEntry(i);
	size_t offset = 0;
	for(Sat::size_type s = 0;s < e.sat.size();++s)
	  if (e.sat[s].empty())
	    offset++; else
	    if (offset > 0)
	      e.sat[s - offset] = e.sat[s];
	assert(offset <= e.sat.size());
	if (offset > 0)
	  e.sat.resize(e.sat.size() - offset);
	clausesRemoved += offset;
      }
  logMsg(LOG_DEBUG, "solver:%zu clauses filtered out", clausesRemoved);
  //Invoking BFS;
  p.clearAllMarks();
  Bfs(p).go(m_userTaskInstall, m_userTaskRemove);
  //Actual removing;
  size_t remNum = 0;
  for(VarId i = 0;i < p.size();++i)
    if (p.hasEntry(i))
      {
	if (!p.getEntry(i).bfsMark)
	  {
	    p.erase(i);
	    remNum++;
	  }
      }
  logMsg(LOG_DEBUG, "solver:%zu involved packages could be safely excluded", remNum);
  if (remNum == 0)
    break;
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

void SatBuilder::use(VarId varId)
{
  assert(varId != BadVarId);
  m_pending.push_back(varId);
}

void SatBuilder::toBeInstalled(VarId varId)
{
  toBeInstalled(varId, BadVarId);
}

void SatBuilder::toBeInstalled(VarId varId, VarId replacementFor)
{
  assert(varId != BadVarId && !m_scope.isInstalled(varId));
  if (p.hasEntry(varId))
    {
      if (replacementFor != BadVarId)
	{
      RefCountedEntry& e = p.getEntry(varId);
      if (e.replacementFor == BadVarId)
e.replacementFor = replacementFor; else
	logMsg(LOG_WARNING, "%s desired to be a replacement for %s but was considered as a replacement for %s previously, left unchanged", m_scope.getDesignation(varId).c_str(), m_scope.getDesignation(replacementFor).c_str(), m_scope.getDesignation(e.replacementFor).c_str());
	}
      return;
    }
  if (m_userTaskRemove.find(varId) != m_userTaskRemove.end())
    {
#ifdef UNBOUNDED_LOG
      logMsg(LOG_DEBUG, "solver:skipping install-type sat construction for %s as it is marked to be removed anyway by user task", m_scope.getDesignation(varId).c_str());
#endif //UNBOUNDED_LOG;
      return;
    }
#ifdef UNBOUNDED_LOG
  if (replacementFor != BadVarId)
    logMsg(LOG_DEBUG, "solver:install-type sat construction for %s as a replacement for %s", m_scope.getDesignation(varId).c_str(), m_scope.getDesignation(replacementFor).c_str()); else
    logMsg(LOG_DEBUG, "solver:install-type sat construction for %s", m_scope.getDesignation(varId).c_str());
#endif //UNBOUNDED_LOG;
  RefCountedEntry& entry = p.newEntry(varId, 0);//0 means currently not installed;
  if (replacementFor != BadVarId)
    entry.replacementFor = replacementFor;
  //Blocking other versions of this package (FIXME:Not every package requires that);
  VarIdVector otherVer;
  m_scope.selectMatchingVarsRealNames(m_scope.pkgIdOfVarId(varId), otherVer);
  noDoubling(otherVer);
  for(VarIdVector::size_type i = 0;i < otherVer.size();++i)
    if (otherVer[i] != varId)
      {
	use(otherVer[i]);
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
      if (alternatives.empty())
	{
	  logMsg(LOG_ERR, "unmet %s in package %s", m_scope.getDesignation(requires[i]).c_str(), m_scope.getDesignation(varId).c_str());
      assert(0);//FIXME:Exception;
	}
      noDoubling(alternatives);
      entry.onRequire(alternatives);
      for(VarIdVector::size_type k = 0;k < alternatives.size();++k)
	use(alternatives[k]);
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
	    use(vars[k]);
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
      use(vars[i]);
    }
}

void SatBuilder::toBeRemoved(VarId varId)
{
  assert(varId != BadVarId && m_scope.isInstalled(varId));
  if (p.hasEntry(varId))
    return;
  if (m_userTaskInstall.find(varId) != m_userTaskInstall.end())
    {
#ifdef UNBOUNDED_LOG
      logMsg(LOG_DEBUG, "solver:skipping remove-type sat construction for %s as it is marked to be installed anyway by user task", m_scope.getDesignation(varId).c_str());
#endif //UNBOUNDED_LOG;
      return;
    }
#ifdef UNBOUNDED_LOG
  logMsg(LOG_DEBUG, "solver:remove-type sat construction for %s", m_scope.getDesignation(varId).c_str());
#endif //UNBOUNDED_LOG;
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
      use(dependent[i]);
      for(VarIdVector::size_type k = 0;k < alternatives.size();++k)
	use(alternatives[k]);
    }
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
      assert(m_replPending.find(varId) == m_replPending.end());
      m_replPending.insert(VarIdToVarIdMap::value_type(varId, replacements.front()));
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
  VarIdVector fixedToInstall;
  solveSat(builder.p, builder.userTaskInstall(), builder.userTaskRemove(), fixedToInstall);
  filterSolution(builder.p, builder.userTaskInstall(), builder.userTaskRemove(), fixedToInstall);

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

  for(size_t i = 0;i < builder.p.size();++i)
    {
      if (!builder.p.hasEntry(i))
	continue;
      const RefCountedEntry& e = builder.p.getEntry(i);
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
    if (builder.p.hasEntry(i))
      {
	const RefCountedEntry& e = builder.p.getEntry(i);
	assert(e.varId == i);

	s << "# " << m_scope.getDesignation(e.varId) << std::endl;
	s << "# Old installation state: " << (e.oldState?"yes":"no") << std::endl;
	s << "# New installation state: " << (e.newState?"yes":"no") << std::endl;
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
		  s << "#";
		if (neg)
		  s << "!";
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
	    if (e.oldState != e.newState && e.oldState)
	      {
		for(Sat::size_type s = 0;s < e.sat.size();++s)
		  {
		    Clause::size_type c;
		    for(c = 0;c < e.sat[s].size();++c)
		      if (!e.sat[s][c].neg &&
			  p.getEntry(e.sat[s][c].varId).newState)
			break;
		    if 	      (c < e.sat[s].size())
		      for(c = 0;c < e.sat[s].size();++c)
			if (e.sat[s][c].neg &&
			    !e.sat[s][c].ambiguous)
			  {
			    e.sat[s][c].ambiguous = 1;
			    releasedReferenceCount++;
			  }
		  }
		continue;
	      }
	    if(e.oldState != e.newState)
	      continue;

	    e.ambiguous = 1;
	    for(Sat::size_type s = 0;s < e.sat.size();++s)
	      for(Clause::size_type c = 0;c < e.sat[s].size();++c)
		if (e.sat[s][c].varId != e.varId)
		  //		  {
		  //		    const VarId varId = e.sat[s][c].varId;
		  releasedReferenceCount++;
	    //		  }
	  }

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
