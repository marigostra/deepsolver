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
#include"TaskSolver.h"
#include"AbstractSatSolver.h"
#include"TaskTranslatorOutputOptimization.h"

void TaskSolver::solve(const UserTask& task,
		       VarIdVector& toInstall,
		       VarIdVector& toRemove)
{
  toInstall.clear();
  toRemove.clear();
  TaskTranslatorOutputOptimization optimization(*this);
  TaskTranslator translator(optimization, m_taskSolverData);
  translator.translate(task);
  if (m_sat.empty())
    return;
  std::auto_ptr<AbstractSatSolver> satSolver = createLibMinisatSolver();
  AbstractSatSolver::VarIdToBoolMap res, assumptions;
  for(Sat::size_type i = 0;i < m_sat.size();i++)
      satSolver->addClause(m_sat[i]);
  VarIdVector solutionConflicts;
  logMsg(LOG_DEBUG, "solver:launching minisat with %zu clauses", m_sat.size());
  if (!satSolver->solve(assumptions, res, solutionConflicts))
    {
      logMsg(LOG_DEBUG, "solver:minisat fails with %zu conflicts:", solutionConflicts.size());
      for(VarIdVector::size_type i = 0;i < solutionConflicts.size();i++)
	logMsg(LOG_DEBUG, "solver:%s", m_scope.constructPackageName(solutionConflicts[i]).c_str());
      throw TaskException(TaskException::NoSatSolution);
    }
  for(AbstractSatSolver::VarIdToBoolMap::const_iterator it = res.begin();it != res.end();it++)
    if (it->second)
      {
	if (!m_scope.isInstalled(it->first))
	  toInstall.push_back(it->first); 
      }else
      {
	if (m_scope.isInstalled(it->first))
	  toRemove.push_back(it->first);
      }
  logMsg(LOG_INFO, "solver:solution found: %zu to install, %zu to remove", toInstall.size(), toRemove.size());
}

void TaskSolver::onUserTaskInstall(VarId varId)
{
  assert(varId != BAD_VAR_ID);
  m_sat.push_back(unitClause(Lit(varId)));
}

void TaskSolver::onUserTaskRemove(VarId varId)
{
  assert(varId != BAD_VAR_ID);
  m_sat.push_back(unitClause(Lit(varId, 1)));
}

void TaskSolver::onBlockingByOtherVersion(VarId varId, VarId blockedBy)
{
  assert(varId != BAD_VAR_ID && blockedBy != BAD_VAR_ID);
  m_sat.push_back(unitClause(Lit(varId, 1)));
}

void TaskSolver::onlyOneMayBeInstalled(VarId varId1, VarId varId2)
{
  assert(varId1 != BAD_VAR_ID && varId2 != BAD_VAR_ID);
  Clause clause;
  clause.push_back(Lit(varId1, 1));
  clause.push_back(Lit(varId2, 1));
  m_sat.push_back(clause);
}

void TaskSolver::requiredOneAnyForPackage(VarId requiredBy, const VarIdVector& alternatives)
{
  assert(requiredBy != BAD_VAR_ID);
  assert(!alternatives.empty());
  Clause clause;
  clause.push_back(Lit(requiredBy, 1));
  for(VarIdVector::size_type i = 0;i < alternatives.size();i++)
    clause.push_back(Lit(alternatives[i]));
  m_sat.push_back(clause);
}

void TaskSolver::onlyOneInstallAnyway(VarId varId1, VarId varId2)
{
  assert(varId1 != BAD_VAR_ID && varId2 != BAD_VAR_ID);
  Clause clause;
  clause.push_back(Lit(varId1, 1));
  clause.push_back(Lit(varId2, 1));
  m_sat.push_back(clause);
  clause.clear();
  clause.push_back(Lit(varId1));
  clause.push_back(Lit(varId2));
  m_sat.push_back(clause);
}

void TaskSolver::notifyUnknownPackageToRemove(const std::string& packageName)
{
  //FIXME:
}

std::auto_ptr<AbstractTaskSolver> createTaskSolver(TaskSolverData& taskSolverData)
{
  return std::auto_ptr<AbstractTaskSolver>(new TaskSolver(taskSolverData));
}
