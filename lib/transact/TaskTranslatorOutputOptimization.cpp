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
#include"TaskTranslatorOutputOptimization.h"

#define INSTALLED(x) (m_installAnyway.find(x) != m_installAnyway.end())
#define REMOVED(x) (m_removeAnyway.find(x) != m_removeAnyway.end())

bool TaskTranslatorOutputOptimization::wasConsideredInstallAnyway(VarId varId) const
{
  return m_installAnyway.find(varId) != m_installAnyway.end();
}

bool TaskTranslatorOutputOptimization::wasConsideredRemoveAnyway(VarId varId) const
{
  return m_removeAnyway.find(varId) != m_removeAnyway.end();
}

void TaskTranslatorOutputOptimization::onUserTaskInstall(VarId varId)
{
  m_installAnyway.insert(varId);
  m_output.onUserTaskInstall(varId);
}

void TaskTranslatorOutputOptimization::onUserTaskRemove(VarId varId)
{
  m_removeAnyway.insert(varId);
  m_output.onUserTaskRemove(varId);
}

void TaskTranslatorOutputOptimization::onBlockingByOtherVersion(VarId varId, VarId blockedBy)
{
  m_removeAnyway.insert(varId);
  m_output.onBlockingByOtherVersion(varId, blockedBy);
}

void TaskTranslatorOutputOptimization::onlyOneMayBeInstalled(VarId varId1, VarId varId2)
{
  if (INSTALLED(varId1))
    m_removeAnyway.insert(varId2);
  if (INSTALLED(varId2))
    m_removeAnyway.insert(varId1);
  m_output.onlyOneMayBeInstalled(varId1, varId2);
}

void TaskTranslatorOutputOptimization::requiredOneAnyForPackage(VarId requiredBy, const VarIdVector& alternatives)
{
  if (INSTALLED(requiredBy) && alternatives.size() == 1)
    m_installAnyway.insert(alternatives[0]);
  if (alternatives.size() == 1 && REMOVED(alternatives[0]))
    m_removeAnyway.insert(requiredBy);
  m_output.requiredOneAnyForPackage(requiredBy, alternatives);
}

void TaskTranslatorOutputOptimization::onlyOneInstallAnyway(VarId varId1, VarId varId2)
{
  if (REMOVED(varId1))
    m_installAnyway.insert(varId2);
  if (REMOVED(varId2))
    m_installAnyway.insert(varId1);
  m_output.onlyOneInstallAnyway(varId1, varId2);
}

void TaskTranslatorOutputOptimization::notifyUnknownPackageToRemove(const std::string& packageName)
{
  m_output.notifyUnknownPackageToRemove(packageName);
}
