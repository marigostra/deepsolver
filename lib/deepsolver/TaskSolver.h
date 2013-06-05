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

#ifndef DEEPSOLVER_TASK_SOLVER_H
#define DEEPSOLVER_TASK_SOLVER_H
#include"AbstractSatSolver.h"

#include"deepsolver/AbstractTaskSolver.h"
#include"deepsolver/TaskTranslator.h"

namespace Deepsolver
{
  class TaskSolver
    : public AbstractTaskSolver,
      private AbstractTaskTranslatorOutput
  {
  public:
    TaskSolver(const TaskSolverData& data)
      : m_taskSolverData(data),
	m_backEnd(data.backEnd),
	m_scope(data.scope) {}

    /**\brief The destructor*/
    virtual ~TaskSolver() {}

  public:
    void solve(const UserTask& task,
	       VarIdVector& toInstall,
	       VarIdVector& toRemove);

  private: //AbstractTaskTranslatorOutput;
    bool wasConsideredInstallAnyway(VarId varId) const {assert(0);}
    bool wasConsideredRemoveAnyway(VarId varId) const {assert(0);}
    void onUserTaskInstall(VarId varId);
    void onUserTaskRemove(VarId varId);
    void onBlockingByOtherVersion(VarId varId, VarId blockedBy);
    void onlyOneMayBeInstalled(VarId varId1, VarId varId2);
    void requiredOneAnyForPackage(VarId requiredBy, const VarIdVector& alternatives);
    void onlyOneInstallAnyway(VarId varId1, VarId varId2);
    void notifyUnknownPackageToRemove(const std::string& packageName);

  private:
    const TaskSolverData& m_taskSolverData;
    const AbstractPackageBackEnd& m_backEnd;
    const AbstractPackageScope& m_scope; 
    Sat m_sat;
  }; //class TaskSolver;
} //namespace Deepsolver;

#endif //DEEPSOLVER_TASK_SOLVER_H;
