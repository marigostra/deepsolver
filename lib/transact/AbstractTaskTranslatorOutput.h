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

#ifndef DEEPSOLVER_ABSTRACT_TASK_TRANSLATOR_OUTPUT_H
#define DEEPSOLVER_ABSTRACT_TASK_TRANSLATOR_OUTPUT_H

#include"AbstractPackageBackEnd.h"
class AbstractTaskTranslatorOutput
{
public:
  AbstractTaskTranslatorOutput() {}
  virtual ~AbstractTaskTranslatorOutput() {}

public:

  virtual bool wasConsideredInstallAnyway(VarId varId) const = 0;
  virtual bool wasConsideredRemoveAnyway(VarId varId) const = 0;
  virtual void onUserTaskInstall(VarId varId) = 0;
  virtual void onUserTaskRemove(VarId varId) = 0;
  virtual void onBlockingByOtherVersion(VarId varId, VarId blockedBy) = 0;
  virtual void onlyOneMayBeInstalled(VarId varId1, VarId varId2) = 0;
  virtual void requiredOneAnyForPackage(VarId requiredBy, const VarIdVector& alternatives) = 0;
  virtual void onlyOneInstallAnyway(VarId varId1, VarId varId2) = 0;
  virtual void notifyUnknownPackageToRemove(const std::string& packageName) = 0;
}; //class AbstractTaskTranslatorOutput;

#endif //DEEPSOLVER_ABSTRACT_TASK_TRANSLATOR_OUTPUT_H;
