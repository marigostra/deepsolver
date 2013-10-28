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

#include"deepsolver/deepsolver.h"
#include"deepsolver/Sat.h"
#include<minisat.h>

DEEPSOLVER_BEGIN_SAT_NAMESPACE

class LibMinisatSolver: public AbstractSatSolver
{
public:
  LibMinisatSolver() 
    : m_nextFreeVar(1) {}

  virtual ~LibMinisatSolver() {}

public:
  void reset();
  void addClause(const Clause& clause);
  bool solve(VarIdToBoolMap& res, VarIdVector& conflicts);

private:
  int mapVarId(VarId varId);

private:
  typedef std::map<VarId, int> VarIdToIntMap;
  typedef std::map<int, VarId> IntToVarIdMap;

private:
  int m_nextFreeVar;//Always greater than real variable count by one;
  VarIdToIntMap m_varIdToIntMap;
  IntToVarIdMap m_intToVarIdMap;
  IntVector m_clauses;
  SizeVector m_clauseSizes;
}; //class LibMinisatSolver;

class LibMinisatCallData
{
public:
  LibMinisatCallData()
    : m_clauseSizes(NULL),
      m_equation(NULL),
      m_solution(NULL),
      m_collisions(NULL) {}

  virtual ~LibMinisatCallData()
  {
    if (m_clauseSizes != NULL)
      delete[] m_clauseSizes;
    if (m_equation != NULL)
      {
	for(size_t i = 0;i < m_clauseCount;i++)
	  if (m_equation[i] != NULL)
	    delete[] m_equation[i];
	delete[] m_equation;
      }
    if (m_solution != NULL)
      delete[] m_solution;
    if (m_collisions != NULL)
      delete[] m_collisions;
  }

public:
  void setClauseSizes(size_t* clauseSizes)
  {
    m_clauseSizes = clauseSizes;
  }

  void setEquation(size_t clauseCount, int** equation)
  {
    m_clauseCount = clauseCount;
    m_equation = equation;
  }

  void setSolution(unsigned char* solution)
  {
    m_solution = solution;
  }

  void setCollisions(unsigned char* collisions)
  {
    m_collisions = collisions;
  }

private:
  size_t* m_clauseSizes;
  size_t m_clauseCount;
  int** m_equation;
  unsigned char* m_solution;
  unsigned char* m_collisions;
}; //class LibMinisatCallData;

void LibMinisatSolver::reset()
{
  m_clauseSizes.clear();
  m_clauses.clear();
  m_varIdToIntMap.clear();
  m_intToVarIdMap.clear();
  m_nextFreeVar = 1;
}

void LibMinisatSolver::addClause(const Clause& clause)
{
  if (clause.empty())
    return;
  for(Clause::size_type i = 0;i < clause.size();i++)
    if (clause[i].neg)
      m_clauses.push_back(-1 * mapVarId(clause[i].varId)); else
      m_clauses.push_back(mapVarId(clause[i].varId));
  m_clauseSizes.push_back(clause.size());
}

bool LibMinisatSolver::solve(VarIdToBoolMap& res, VarIdVector& conflicts)
{
  assert(!m_clauses.empty());
  LibMinisatCallData data;
  size_t* clauseSizes = new size_t[m_clauseSizes.size()];
  data.setClauseSizes(clauseSizes);
  for(SizeVector::size_type i = 0;i < m_clauseSizes.size();i++)
    clauseSizes[i] = m_clauseSizes[i];
  int** equation = new int*[m_clauseSizes.size()];
  for(SizeVector::size_type i = 0;i < m_clauseSizes.size();i++)
    equation[i] = NULL;
  data.setEquation(m_clauseSizes.size(), equation);
  for(size_t i = 0;i < m_clauseSizes.size();i++)
equation[i] = new int[m_clauseSizes[i]];
  IntVector::size_type index = 0;
  for(size_t i = 0;i < m_clauseSizes.size();i++)
    for(size_t j = 0;j < m_clauseSizes[i];j++)
      equation[i][j] = m_clauses[index++];
  assert(index == m_clauses.size());
  unsigned char* solution = new unsigned char[m_nextFreeVar];
  for(size_t i = 0;i < m_nextFreeVar;i++)
    solution[i] = 0;
  data.setSolution(solution);
  unsigned char* collisions = new unsigned char[m_nextFreeVar];
  for(size_t i = 0;i < m_nextFreeVar;i++)
    collisions[i] = 0;
  data.setCollisions(collisions);
  logMsg(LOG_DEBUG, "libminisat:calling libminisat to solve the task with %zu variables in %zu clauses", m_nextFreeVar - 1, m_clauseSizes.size());
  const int code = minisat_solve(m_nextFreeVar,
				 m_clauseSizes.size(),
				 clauseSizes, 
				 equation,
				 solution,
				 collisions);
  logMsg(LOG_DEBUG, "libminisat:minisat_solve() has returned code %d", code);
  if (code == MINISAT_UNSAT)
    {
      conflicts.clear();
      for(size_t i = 1;i < m_nextFreeVar;i++)
	if (collisions[i])
	  {
	    IntToVarIdMap::const_iterator it = m_intToVarIdMap.find(i);
	    assert(it != m_intToVarIdMap.end());
	    conflicts.push_back(it->second);
	  }
      logMsg(LOG_DEBUG, "libminisat:libminisat said no solution with %zu highlighted collisions", conflicts.size());
      return 0;
    }
  if (code != MINISAT_OK)
    {
      logMsg(LOG_DEBUG, "libminisat:libminisat said no solution (collisions information is not accessible)");
      return 0;
    }
  logMsg(LOG_DEBUG, "libminisat:libminisat found a solution!");
  for(size_t i = 1;i < m_nextFreeVar;i++)
    {
      IntToVarIdMap::const_iterator it = m_intToVarIdMap.find(i);
      assert(it != m_intToVarIdMap.end());
      res.insert(VarIdToBoolMap::value_type(it->second, solution[i] != 0));
    }
  return 1;
}

int LibMinisatSolver::mapVarId(VarId varId)
{
  assert(varId != BadVarId);
  VarIdToIntMap::const_iterator it = m_varIdToIntMap.find(varId);
  if (it != m_varIdToIntMap.end())
    {
      assert(it->second != 0);
      return it->second;
    }
  const int newValue = m_nextFreeVar;
  m_varIdToIntMap.insert(VarIdToIntMap::value_type(varId, newValue));
  m_intToVarIdMap.insert(IntToVarIdMap::value_type(newValue, varId));
  m_nextFreeVar++;
  return newValue;
}

std::auto_ptr<AbstractSatSolver> createDefaultSatSolver()
{
  return std::auto_ptr<AbstractSatSolver>(new LibMinisatSolver());
}

DEEPSOLVER_END_SAT_NAMESPACE
