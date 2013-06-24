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

#ifndef DEEPSOLVER_PKG_UTILS_H
#define DEEPSOLVER_PKG_UTILS_H

#include"deepsolver/AbstractPackageBackEnd.h"
#include"deepsolver/AbstractPackageScope.h"
#include"deepsolver/AbstractSatSolver.h"
#include"deepsolver/PkgSnapshot.h"
#include"deepsolver/ProvideMap.h"
#include"deepsolver/InstalledReferences.h"

namespace Deepsolver
{
  class PkgUtils
  {
  public:
    static void fillWithhInstalledPackages(AbstractPackageBackEnd& backEnd,
					   PkgSnapshot::Snapshot& snapshot,
					   ConstCharVector& strings,
					   bool stopOnInvalidPkg);

    static void prepareReversedMaps(const PkgSnapshot::Snapshot& snapshot,
				    ProvideMap& provideMap,
				    InstalledReferences& requires,
				    InstalledReferences& conflicts);

    static void fillUpgradeDowngrade(const AbstractPackageBackEnd& backEnd,
				     const AbstractPackageScope& scope,
				     VarIdVector& install,
				     VarIdVector& remove,
				     VarIdToVarIdMap& upgrade,
				     VarIdToVarIdMap& downgrade);
  }; //class PkgUtils;
} //namespace Deepsolver;

#endif //DEEPSOLVER_PKG_UTILS_H;
