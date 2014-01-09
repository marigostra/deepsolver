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

#ifndef DEEPSOLVER_PKG_SCOPE_H
#define DEEPSOLVER_PKG_SCOPE_H

#include"deepsolver/PkgScopeMetadata.h"

namespace Deepsolver
{
  class PkgScope: public PkgScopeMetadata
  {
  public:
    PkgScope(const AbstractPkgBackEnd& backend, const Snapshot& snapshot)
      : PkgScopeMetadata(backend, snapshot) {}

    /**\brief The destructor*/
  virtual ~PkgScope() {}

  public:
    void selectMatchingVarsProvidesOnly(const IdPkgRel& rel, VarIdVector& vars) const override;
    void selectMatchingVarsProvidesOnly(PackageId packageId, VarIdVector& vars) const override;

    void selectMatchingVarsProvidesOnly(PackageId packageId,
					 const VerSubset& ver,
					 VarIdVector& vars) const override;

    void selectMatchingVarsRealNames(const IdPkgRel& rel, VarIdVector& vars) const override;
    void selectMatchingVarsRealNames(PackageId packageId, VarIdVector& vars) const override;

    void selectMatchingVarsRealNames(PackageId packageId,
				     const VerSubset& ver,
				     VarIdVector& vars) const override;

    void selectMatchingVarsWithProvides(const IdPkgRel& rel, VarIdVector& vars) const override;
    void selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars) const override;

    void selectMatchingVarsWithProvides(PackageId packageId,
					const VerSubset& ver,
					VarIdVector& vars) const override;

    bool isInstalled(VarId varId) const override;
    void selectTheNewest(VarIdVector& vars) const override;
    void selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry) const override;
    bool allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry) const override;
    void getRequires(VarId varId, IdPkgRelVector& res) const override;
    void getConflicts(VarId varId, IdPkgRelVector& res) const override;

    void whatConflictAmongInstalled(VarId varId,
				     VarIdVector& res,
				     IdPkgRelVector& resRels) const override;

    void whatDependAmongInstalled(VarId varId,
				   VarIdVector& res,
				   IdPkgRelVector& resRels) const override;

    void whatSatisfyAmongInstalled(const IdPkgRel& rel, VarIdVector& res) const override;

  private:
    void getRequires(VarId varId,
		     PackageIdVector& depWithoutVersion,
		     PackageIdVector& depWithVersion,
		     VerSubsetVector& versions) const;

    void selectVarsToTry(PkgId pkgId,
			 VarIdVector& toTry,
			 bool includeItself) const;

    void getConflicts(VarId varId,
		      PackageIdVector& withoutVersion,
		      PackageIdVector& withVersion,
		      VerSubsetVector& versions) const;
  }; //class PkgScope;
} //namespace Deepsolver;

#endif //DEEPSOLVER_PKG_SCOPE_H;
