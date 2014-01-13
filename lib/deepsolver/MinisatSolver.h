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

#ifndef DEEPSOLVER_MINISAT _SOLVER_H
#define DEEPSOLVER_MINISAT _SOLVER_H

#include"deepsolver/Sat.h"

namespace Deepsolver
{
  namespace Sat
  {
    class MinisatSolver: public AbstractSatSolver
    {
    public:
      /**\brief The default constructor*/
      MinisatSolver() 
	: m_nextFreeVar(1) {}

      /**\brief The destructor*/
      virtual ~MinisatSolver() {}

    public:
      void reset() override;
      void addClause(const Clause& clause) override;
      bool solve(VarIdToBoolMap& res, VarIdVector& conflicts) override;

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
    }; //class MinisatSolver;
  } //namespace Sat;
} //namespace Deepsolver;

#endif //DEEPSOLVER_MINISAT _SOLVER_H;
