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

#include"deepsolver/AbstractPkgBackEnd.h"
#include"deepsolver/SolverBase.h"
#include"deepsolver/Sat.h"
#include"deepsolver/AbstractTaskSolver.h"

namespace Deepsolver
{
  namespace Solver
  {
    class RefCountedEntry
    {
    public:
      RefCountedEntry(VarId v, bool s)
	: varId(v),
	  oldState(s),
	  newState(0),
	  refNum(0) {}

      virtual ~RefCountedEntry() {}

    public:
      void onConflict(VarId v)
      {
	assert(v != BadVarId);
	Sat::Clause c;
	c.push_back(Sat::Lit(varId, 1));
	c.push_back(Sat::Lit(v, 1));
	sat.push_back(c);
      }

      void onRequire(const VarIdVector& alternatives)
      {
	if (alternatives.empty())
	  {
	    logMsg(LOG_WARNING, "solver:trying to add a require without alternatives");
	    return;
	  }
	Sat::Clause c;
	c.push_back(Sat::Lit(varId, 1));
	for(VarIdVector::size_type i = 0;i < alternatives.size();++i)
	  {
	    assert(alternatives[i] != BadVarId);
	    c.push_back(Sat::Lit(alternatives[i]));
	  }
	sat.push_back(c);
      }

      void onDependent(VarId dependent, const VarIdVector& alternatives)
      {
	assert(dependent != BadVarId);
	Sat::Clause c;
	c.push_back(Sat::Lit(varId));
	c.push_back(Sat::Lit(dependent, 1));
	for(VarIdVector::size_type i = 0;i < alternatives.size();++i)
	  {
	    assert(alternatives[i] != BadVarId);
	    c.push_back(Sat::Lit(alternatives[i]));
  }
	sat.push_back(c);
      }

public:
      VarId varId;
      bool oldState, newState;
      size_t refNum;
      Sat::Sat sat;
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

      void clear()
      {
	for(RefCountedEntryVector::size_type i = 0;i < m_entries.size();++i)
	  delete m_entries[i];
	m_entries.clear();
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
	assert(varId < m_entries.size());
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

      const RefCountedEntry& getEntry(VarId varId) const
      {
	assert(varId < m_entries.size());
	return *m_entries[varId];
      }

      RefCountedEntry& newEntry(VarId varId, bool installed)
      {
	assert(!hasEntry(varId));
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
      void fillEntry(VarId varId);
      VarId onUserItemToInstall(const UserTaskItemToInstall& item) const;
      void toBeInstalled(VarId VarId);
      void toBeRemoved(VarId varId);
      VarId byProvidesPriorityList(const VarIdVector& vars, PkgId provideEntry) const;
      VarId byProvidesPrioritySorting(const VarIdVector& vars) const;

    public:
      RefCountedEntries p;

    private:
      const AbstractPkgBackEnd& m_backend;
      const AbstractPkgScope& m_scope; 
      const AbstractProvidePriority& m_providePriority;
      VarIdSet m_userTaskInstall, m_userTaskRemove;
    }; //class SatBuilder;

    class Solver: public AbstractTaskSolver
    {
    public:
      Solver(const TaskSolverData& taskSolverData)
	: m_taskSolverData(taskSolverData) {}

      virtual ~Solver() {}

    public:
      void solve(const UserTask& userTask, 
			 VarIdVector& install,
			 VarIdVector& remove) const override;

    private:
      bool solveSat(RefCountedEntries& p,
			const VarIdSet& userTaskInstall,
			const VarIdSet& userTaskRemove) const;

      void filterSolution(RefCountedEntries& p) const;

    private:
      const TaskSolverData& m_taskSolverData;
    }; //class Solver;
  } //namespace Solver;
} //namespace Deepsolver;

#endif //DEEPSOLVER_SOLVER_H;