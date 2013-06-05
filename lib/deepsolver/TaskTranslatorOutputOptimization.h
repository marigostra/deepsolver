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

#ifndef DEEPSOLVER_TASK_TRANSLATOR_OUTPUT_OPTIMIZATION_H
#define DEEPSOLVER_TASK_TRANSLATOR_OUTPUT_OPTIMIZATION_H

#include"deepsolver/AbstractTaskTranslatorOutput.h"

namespace Deepsolver
{
  class TaskTranslatorOutputOptimization: public AbstractTaskTranslatorOutput
  {
  public:
    TaskTranslatorOutputOptimization(AbstractTaskTranslatorOutput& output) 
      : m_output(output) {}

    /**\brief The destructor*/
  virtual ~TaskTranslatorOutputOptimization() {}

  public:
    bool wasConsideredInstallAnyway(VarId varId) const;
    bool wasConsideredRemoveAnyway(VarId varId) const;
    void onUserTaskInstall(VarId varId);
    void onUserTaskRemove(VarId varId);
    void onBlockingByOtherVersion(VarId varId, VarId blockedBy);
    void onlyOneMayBeInstalled(VarId varId1, VarId varId2);
    void requiredOneAnyForPackage(VarId requiredBy, const VarIdVector& alternatives);
    void onlyOneInstallAnyway(VarId varId1, VarId varId2);
    void notifyUnknownPackageToRemove(const std::string& packageName);

  private:
    AbstractTaskTranslatorOutput& m_output;
    VarIdSet m_installAnyway, m_removeAnyway;
  }; //class TaskTranslatorOutputOptimization;
} //namespace Deepsolver;

#endif //DEEPSOLVER_TASK_TRANSLATOR_OUTPUT_OPTIMIZATION_H;
