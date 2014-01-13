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

#include"deepsolver/deepsolver.h"
#include"deepsolver/Sat.h"
#include"deepsolver/MinisatSolver.h"

DEEPSOLVER_BEGIN_SAT_NAMESPACE

AbstractSatSolver::Ptr createDefaultSatSolver()
{
  return AbstractSatSolver::Ptr(new MinisatSolver());
}

DEEPSOLVER_END_SAT_NAMESPACE
