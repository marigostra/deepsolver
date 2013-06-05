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

#ifndef DEEPSOLVER_PKG_SCOPE_H
#define DEEPSOLVER_PKG_SCOPE_H

#include"deepsolver/PkgScopeBase.h"

namespace Deepsolver
{
  class PkgScope: public PkgScopeBase
  {
  public:
    PkgScope(const AbstractPackageBackEnd& backEnd,
	     const Snapshot& snapshot,
	     const ProvideMap& provideMap,
	     const InstalledReferences& installedRequiresEntries,
	     const InstalledReferences& installedConflictsEntries)
      : PkgScopeBase(backEnd, snapshot, provideMap, installedRequiresEntries, installedConflictsEntries) {}

    /**\brief The destructor*/
  virtual ~PkgScope() {}

  public:
    void selectMatchingVarsAmongProvides(const IdPkgRel& rel, VarIdVector& vars) const;
    void selectMatchingVarsAmongProvides(PackageId packageId, VarIdVector& vars) const;

    void selectMatchingVarsAmongProvides(PackageId packageId,
					 const VersionCond& ver,
					 VarIdVector& vars) const;

    void selectMatchingVarsRealNames(const IdPkgRel& rel, VarIdVector& vars) const;
    void selectMatchingVarsRealNames(PackageId packageId, VarIdVector& vars) const;

    void selectMatchingVarsRealNames(PackageId packageId,
				     const VersionCond& ver,
				     VarIdVector& vars) const;

    void selectMatchingVarsWithProvides(const IdPkgRel& rel, VarIdVector& vars) const;
    void selectMatchingVarsWithProvides(PackageId packageId, VarIdVector& vars) const;

    void selectMatchingVarsWithProvides(PackageId packageId,
					const VersionCond& ver,
					VarIdVector& vars) const;

    bool isInstalled(VarId varId) const;
    void selectTheNewest(VarIdVector& vars) const;
    void selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry) const;
    bool allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry) const;
    void getRequires(VarId varId, IdPkgRelVector& res) const;
    void getConflicts(VarId varId, IdPkgRelVector& res) const;

    void whatConflictsAmongInstalled(VarId varId,
				     VarIdVector& res,
				     IdPkgRelVector& resRels) const;

    void whatDependsAmongInstalled(VarId varId,
				   VarIdVector& res,
				   IdPkgRelVector& resRels) const;

    void whatSatisfiesAmongInstalled(const IdPkgRel& rel, VarIdVector& res) const;

  private:
    void getRequires(VarId varId,
		     PackageIdVector& depWithoutVersion,
		     PackageIdVector& depWithVersion,
		     VersionCondVector& versions) const;

    void selectVarsToTry(PkgId pkgId,
			 VarIdVector& toTry,
			 bool includeItself) const;

    void getConflicts(VarId varId,
		      PackageIdVector& withoutVersion,
		      PackageIdVector& withVersion,
		      VersionCondVector& versions) const;

  private:
    static VersionCond constructVersionCondEquals(int epoch, 
						  const std::string& version,
						  const std::string& release);
  }; //class PkgScope;
} //namespace Deepsolver;

#endif //DEEPSOLVER_PKG_SCOPE_H;
