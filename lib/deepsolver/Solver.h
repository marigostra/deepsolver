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

#ifndef DEEPSOLVER_SOLVER_H
#define DEEPSOLVER_SOLVER_H

//#define DEEPSOLVER_SOLVER_DEBUG

#include"deepsolver/AbstractPkgBackEnd.h"
#include"deepsolver/SolverBase.h"
#include"deepsolver/Sat.h"
#include"deepsolver/AbstractTaskSolver.h"

namespace Deepsolver
{
  namespace Solver
  {
    using Sat::Lit;
    using Sat::Clause;
    using Sat::unitClause;
    using Sat::AbstractSatSolver;
    using Sat::createDefaultSatSolver;
    using Sat::Sat;
    typedef AbstractSatSolver::VarIdToBoolMap VarIdToBoolMap;

    class RefCountedEntry
    {
    public:
      RefCountedEntry(VarId v, bool s)
	: varId(v),
	  oldState(s),
	  newState(s),
	  refNum(0), 
	  germ(0),
	  ambiguous(0),
	  bfsMark(0),
	  replacementFor(BadVarId) {}

      virtual ~RefCountedEntry() {}

    public:
      void uninstallable()
      {
	sat.push_back(unitClause(Lit(varId, 1)));
      }

      void unremovable()
      {
	sat.push_back(unitClause(Lit(varId)));
      }

      void onConflict(VarId v)
      {
	assert(v != BadVarId);
	Clause c;
	c.push_back(Lit(varId, 1));
	c.push_back(Lit(v, 1));
	sat.push_back(c);
      }

      void onRequire(const VarIdVector& alternatives)
      {
	if (alternatives.empty())
	  {
	    logMsg(LOG_WARNING, "solver:trying to add a require without alternatives");
	    return;
	  }
	Clause c;
	c.push_back(Lit(varId, 1));
	for(VarIdVector::size_type i = 0;i < alternatives.size();++i)
	  {
	    assert(alternatives[i] != BadVarId);
	    c.push_back(Lit(alternatives[i]));
	  }
	sat.push_back(c);
      }

      void onDependent(VarId dependent, const VarIdVector& alternatives)
      {
	assert(dependent != BadVarId);
	Clause c;
	c.push_back(Lit(varId));
	c.push_back(Lit(dependent, 1));
	for(VarIdVector::size_type i = 0;i < alternatives.size();++i)
	  {
	    assert(alternatives[i] != BadVarId);
	    c.push_back(Lit(alternatives[i]));
  }
	sat.push_back(c);
      }

      void clearAllMarks()
      {
	ambiguous = 0;
	bfsMark = 0;
	for(Sat::size_type i = 0;i < sat.size();++i)
	  for(Clause::size_type j = 0;j < sat[i].size();++j)
	    sat[i][j].ambiguous = 0;
      }

    public:
      VarId varId;
      bool oldState, newState;
      size_t refNum;
      Sat sat;
      bool germ;
      VarIdVector reconstruct;
      bool ambiguous, bfsMark;
      VarId replacementFor;
    }; //class RefCountedEntry;

    typedef std::vector<RefCountedEntry*> RefCountedEntryVector;
    typedef std::list<RefCountedEntry*> RefCountedEntryList;

    class RefCountedEntries
    {
    public:
      RefCountedEntries() {}

      virtual ~RefCountedEntries() 
      {
	clear();
      }

    public:
      void init(size_t entryCount)
      {
	clear();
	m_entries.resize(entryCount);
	for(RefCountedEntryVector::size_type i = 0;i < m_entries.size();++i)
	  m_entries[i] = NULL;
      }

      void erase(VarId varId)
      {
	assert(varId != BadVarId && hasEntry(varId));
	delete m_entries[varId];
	m_entries[varId] = NULL;
      }

      void clear()
      {
	for(RefCountedEntryVector::size_type i = 0;i < m_entries.size();++i)
	  delete m_entries[i];
	m_entries.clear();
      }

      void clearGerms()
      {
	for(RefCountedEntryVector::size_type i = 0;i < m_entries.size();++i)
	  if (m_entries[i] != NULL && m_entries[i]->germ)
	    {
	      delete m_entries[i];
	      m_entries[i] = NULL;
	    }
      }

      void clearBfsMarks()
      {
	for(RefCountedEntryVector::size_type i = 0;i < m_entries.size();++i)
	  if (m_entries[i] != NULL)
	    m_entries[i]->bfsMark = 0;
      }

      void clearAllMarks()
      {
	for(RefCountedEntryVector::size_type i = 0;i < m_entries.size();++i)
	  if (m_entries[i] != NULL)
	    m_entries[i]->clearAllMarks();
      }

      bool newStatesValid() const
      {
	size_t count = 0;
	for(VarId i = 0;i < m_entries.size();++i)
	  if (m_entries[i] != NULL)
	    {
	      const RefCountedEntry& e = *m_entries[i];
	      for(Sat::size_type s = 0;s < e.sat.size();++s)
		{
		  Clause::size_type c;
		  for(c = 0;c < e.sat[s].size();++c)
		    {
		      const VarId v = e.sat[s][c].varId;
		      const bool n = e.sat[s][c].neg;
		      assert(hasEntry(v));
		      if (!n && getEntry(v).newState)
			break;
		      if (n && !getEntry(v).newState)
			break;
		    }
		  if (c >= e.sat[s].size())
		    return 0;
		}
	      count += e.sat.size();
	    } //if(entry exists);
	logMsg(LOG_DEBUG, "solver:%zu clauses are checked and OK!", count);
	return 1;
      }


      size_t size() const
      {
	return m_entries.size();
      }

      bool hasEntry(VarId varId) const
      {
	return varId < m_entries.size() && m_entries[varId] != NULL;
      }

      RefCountedEntry& getEntry(VarId varId)
      {
	assert(varId < m_entries.size() && hasEntry(varId));
	return *m_entries[varId];
      }

      const RefCountedEntry& getEntry(VarId varId) const
      {
	assert(varId < m_entries.size() && hasEntry(varId));
	return *m_entries[varId];
      }

      void incRef(VarId varId)
      {
	assert(hasEntry(varId));
	m_entries[varId]->refNum--;
      }

      void decRef(VarId varId)
      {
	assert(hasEntry(varId) && m_entries[varId]->refNum > 0);
	m_entries[varId]->refNum--;
      }

      RefCountedEntry& newEntry(VarId varId, bool installed)
      {
	assert(varId != BadVarId && !hasEntry(varId));
	assert(varId < m_entries.size());
	m_entries[varId] = new RefCountedEntry(varId, installed);
	return *m_entries[varId];
      }

      RefCountedEntry& ensureEntryExists(VarId varId, bool installed)
      {
	assert(varId != BadVarId);
	if (hasEntry(varId))
	  return getEntry(varId);
	assert(varId < m_entries.size());
	m_entries[varId] = new RefCountedEntry(varId, installed);
	return *m_entries[varId];
      }


    private:
      RefCountedEntryVector m_entries;
    }; //class RefCountedEntries;Ref

    class SatBuilder
    {
    public:
      SatBuilder(const AbstractPkgBackEnd& backend,
		 const AbstractPkgScope& scope,
		 const AbstractProvidePriority& providePriority)
	: m_backend(backend),
	  m_scope(scope),
	  m_providePriority(providePriority) {}

      /**\brief The destructor*/
      virtual ~SatBuilder() {}

    public:
      void build(const UserTask& task);

      const VarIdSet& userTaskInstall() const
      {
	return m_userTaskInstall;
      }

      const VarIdSet& userTaskRemove() const
      {
	return m_userTaskRemove;
      }

    private:
      void onUserTask(const UserTask& userTask);
      void use(VarId varId, VarId referenceFrom);

      //Returns true if germ finally exists and item to reconstruct is added;
      //False only if the package is already fully constructed;
      bool useGermInstalled(VarId varId, VarId reconstruct);

      //Returns true if germ finally exists and item to reconstruct is added;
      //False only if the package is already fully constructed;
      bool useGermUninstalled(VarId varId, VarId reconstruct);
      VarId onUserItemToInstall(const UserTaskItemToInstall& item) const;
      void toBeInstalled(VarId VarId);
      void toBeInstalled(VarId varId, VarId replacementFor);
      void toBeRemoved(VarId varId);
      VarId byProvidesPriorityList(const VarIdVector& vars, PkgId provideEntry) const;
      VarId byProvidesPrioritySorting(const VarIdVector& vars) const;

      void onPending();
      //This method in contrast to onPending() doesn't care whether packages are processed or not;
      void reconstruct(const VarIdVector& vars);

      bool ensureSatCorrect() const;

    public:
      RefCountedEntries p;

    private:
      const AbstractPkgBackEnd& m_backend;
      const AbstractPkgScope& m_scope; 
      const AbstractProvidePriority& m_providePriority;
      VarIdSet m_userTaskInstall, m_userTaskRemove;
      VarIdVector m_pending;
      //      VarIdToVarIdMap m_replPending;
#ifdef DEEPSOLVER_SOLVER_DEBUG
      VarIdVector m_debugReferences;
#endif //DEEPSOLVER_SOLVER_DEBUG;
    }; //class SatBuilder;

    class Solver: public AbstractTaskSolver
    {
    public:
      Solver(const TaskSolverData& taskSolverData)
	: m_taskSolverData(taskSolverData),
	  m_scope(taskSolverData.scope) {}

      virtual ~Solver() {}

    public:
      void solve(const UserTask& userTask, 
			 VarIdVector& install,
			 VarIdVector& remove) const override;

      void dumpSat(const UserTask& userTask, std::ostream& s) const;

    private:
      void doMainWork(SatBuilder& builder, const UserTask& userTask) const;
      bool solveSat(RefCountedEntries& p,
			const VarIdSet& userTaskInstall,
		    const VarIdSet& userTaskRemove,
	const VarIdVector& fixedToInstall) const;

      void filterSolution(RefCountedEntries& p,
			  const VarIdSet& userTaskInstall,
			  const VarIdSet& userTaskRemove,
const VarIdVector& fixedToInstall) const;

    private:
      const TaskSolverData& m_taskSolverData;
      const AbstractPkgScope& m_scope;
    }; //class Solver;
  } //namespace Solver;
} //namespace Deepsolver;

#endif //DEEPSOLVER_SOLVER_H;
