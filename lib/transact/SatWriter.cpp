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
#include"SatWriter.h"
#include"TaskTranslator.h"

std::string SatWriter::generateSat(const UserTask& task)
{
  m_clauseVector.clear();
  m_clauseSet.clear();
  m_optimization = std::auto_ptr<TaskTranslatorOutputOptimization>(new TaskTranslatorOutputOptimization(*this));
  TaskTranslator translator(*m_optimization.get(), m_taskSolverData);
  translator.translate(task);
  assert(m_clauseVector.size() == m_clauseSet.size());
  std::ostringstream ss;
  ss << "# " << m_clauseVector.size() << " clauses" << std::endl;
  std::string res = ss.str();
  for(StringVector::size_type i = 0;i < m_clauseVector.size();i++)
    {
      res += m_clauseVector[i];
      if (i + 1 < m_clauseVector.size())
	res += "&&\n";
    }
  return res;
}

void SatWriter::onUserTaskInstall(VarId varId)
{
  assert(varId != BAD_VAR_ID);
  addClause("(\n  " + m_scope.constructPackageName(varId) + "\n)\n");
}

void SatWriter::onUserTaskRemove(VarId varId)
{
  assert(varId != BAD_VAR_ID);
  addClause("(\n !" + m_scope.constructPackageName(varId) + "\n)\n");
}

void SatWriter::onBlockingByOtherVersion(VarId varId, VarId blockedBy)
{
  assert(varId != BAD_VAR_ID && blockedBy != BAD_VAR_ID);
  addClause("(\n !" + m_scope.constructPackageName(varId) + " # Blocked by \"" + m_scope.constructPackageName(blockedBy) + "\"\n)\n");
}

void SatWriter::onlyOneMayBeInstalled(VarId varId1, VarId varId2)
{
  assert(varId1 != BAD_VAR_ID && varId2 != BAD_VAR_ID);
  std::string res = "(\n";
  res += " !" + m_scope.constructPackageName(varId1) + " ||\n";
  res += " !" + m_scope.constructPackageName(varId2) + "\n";
  res += ")\n";
  addClause(res);
}

void SatWriter::requiredOneAnyForPackage(VarId requiredBy, const VarIdVector& alternatives)
{
  assert(requiredBy != BAD_VAR_ID);
  assert(!alternatives.empty());
  assert(m_optimization.get() != NULL);
  std::string res = "(\n !" + m_scope.constructPackageName(requiredBy) + " ||";
  if (m_optimization->wasConsideredInstallAnyway(requiredBy))
    res += " # Will be installed anyway"; else
    if (m_optimization->wasConsideredRemoveAnyway(requiredBy))
      res += " # Never be installed"; else
      if (m_scope.isInstalled(requiredBy))
	res += " # Installed now";
  res += "\n";
  for(VarIdVector::size_type i = 0;i < alternatives.size();i++)
    {
      res += "  " + m_scope.constructPackageName(alternatives[i]);
      if (i + 1 < alternatives.size())
	res += " ||";
      if (m_optimization->wasConsideredInstallAnyway(alternatives[i]))
	res += " # Will be installed anyway"; else
      if (m_optimization->wasConsideredRemoveAnyway(alternatives[i]))
	res += " # Never be installed"; else
	if (m_scope.isInstalled(alternatives[i]))
	  res += " # Installed now";
	res += "\n";
    }
  res += ")\n";
  addClause(res);
}

void SatWriter::onlyOneInstallAnyway(VarId varId1, VarId varId2)
{
  std::string res = "(\n";
  res += " !" + m_scope.constructPackageName(varId1) + " ||\n";
  res += " !" + m_scope.constructPackageName(varId2) + "\n";
  res += ")\n";
  addClause(res);
  res = "(\n";
  res += "  " + m_scope.constructPackageName(varId1) + " ||\n";
  res += "  " + m_scope.constructPackageName(varId2) + "\n";
  res += ")\n";
  addClause(res);
}

void SatWriter::notifyUnknownPackageToRemove(const std::string& packageName)
{
  //FIXME:
}

void SatWriter::addClause(const std::string& str)
{
  if (m_clauseSet.find(str) != m_clauseSet.end())
    return;
  m_clauseVector.push_back(str);
  m_clauseSet.insert(str);
}
