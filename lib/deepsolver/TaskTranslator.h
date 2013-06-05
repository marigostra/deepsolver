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

#ifndef DEEPSOLVER_TASK_TRANSLATOR_H
#define DEEPSOLVER_TASK_TRANSLATOR_H

#include"deepsolver/AbstractPackageBackEnd.h"
#include"deepsolver/AbstractPackageScope.h"
#include"deepsolver/PackageSelection.h"
#include"deepsolver/AbstractTaskTranslatorOutput.h"
#include"deepsolver/PostponedRequire.h"

namespace Deepsolver
{
  class TaskTranslator
  {
  public:
    TaskTranslator(AbstractTaskTranslatorOutput& output, const TaskSolverData& data)
      : m_output(output),
	m_taskSolverData(data),
	m_backEnd(data.backEnd),
	m_scope(data.scope),
	m_selection(data.backEnd, data.scope, data.provides) {}

    /**\brief The destructor*/
    virtual ~TaskTranslator() {}

  public:
    void translate(const UserTask& task);

  private:
    void translateUserTask(const UserTask& userTask);
    void handleChangeToFalse(VarId VarId);
    void handleChangeToTrue(VarId varId);
    void processPendings();
    std::string relToString(const IdPkgRel& rel);
    void processVersionUpgrade(VarId varId);

  private:
    AbstractTaskTranslatorOutput& m_output;
    const TaskSolverData& m_taskSolverData;
    const AbstractPackageBackEnd& m_backEnd;
    const AbstractPackageScope& m_scope; 
    const PackageSelection m_selection;
    VarIdVector m_pending;
    VarIdSet m_processed;
    PostponedRequire m_postponedRequires;
  }; //class TaskTranslator;
} //namespace Deepsolver;

#endif //DEEPSOLVER_TASK_TRANSLATOR_H;
