/*
   Copyright 2011-2014 ALT Linux
   Copyright 2011-2014 Michael Pozhidaev

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

#ifndef DEEPSOLVER_ABSTRACT_TASK_SOLVER_H
#define DEEPSOLVER_ABSTRACT_TASK_SOLVER_H

#include"deepsolver/AbstractPkgBackEnd.h"
#include"deepsolver/SolverBase.h"

namespace Deepsolver
{
  struct TaskSolverData
  {
    TaskSolverData(const AbstractPkgBackEnd& b,
		   const AbstractPkgScope& s,
		   const Solver::AbstractProvidePriority& p)
      : backend(b),
	scope(s),
	providePriority(p) {}

    const AbstractPkgBackEnd& backend;
    const AbstractPkgScope& scope;
    const Solver::AbstractProvidePriority&providePriority; 
  }; //struct TaskSolverData;

  class AbstractTaskSolver
  {
  public:
    typedef std::shared_ptr<AbstractTaskSolver> Ptr;

  public:
    AbstractTaskSolver() {}
    virtual ~AbstractTaskSolver() {}

  public:
    virtual void solve(const UserTask& userTask, 
		       VarIdVector& install,
		       VarIdVector& remove) const = 0;

    virtual void dumpSat(const UserTask& userTask, std::ostream& s) const = 0;
  }; //class AbstractTaskSolver;

  AbstractTaskSolver::Ptr createTaskSolver(const TaskSolverData& taskSolverData);
} //namespace Deepsolver;

#endif //DEEPSOLVER_ABSTRACT_TASK_SOLVER_H;
