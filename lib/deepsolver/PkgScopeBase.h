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

#ifndef DEEPSOLVER_PKG_SCOPE_BASE_H
#define DEEPSOLVER_PKG_SCOPE_BASE_H

#include"deepsolver/AbstractPackageBackEnd.h"
#include"deepsolver/AbstractPackageScope.h"
#include"deepsolver/PkgSnapshot.h"
#include"deepsolver/ProvideMap.h"
#include"deepsolver/InstalledReferences.h"

namespace Deepsolver
{
  class PkgScopeBase: public AbstractPackageScope
  {
  protected:
    typedef PkgSnapshot::Snapshot Snapshot;
    typedef PkgSnapshot::Pkg SnapshotPkg;
    typedef PkgSnapshot::PkgVector SnapshotPkgVector;
    typedef PkgSnapshot::Relation SnapshotRelation;
    typedef PkgSnapshot::RelationVector SnapshotRelationVector;

  public:
    PkgScopeBase(const AbstractPackageBackEnd& backEnd,
		 const Snapshot& snapshot,
		 const ProvideMap& provideMap,
		 const InstalledReferences& installedRequiresEntries,
		 const InstalledReferences& installedConflictsEntries)
      : m_backEnd(backEnd),
	m_snapshot(snapshot),
	m_pkgs(snapshot.pkgs),
      m_relations(snapshot.relations),
	m_provideMap(provideMap),
	m_installedRequiresEntries(installedRequiresEntries),
	m_installedConflictsEntries(installedConflictsEntries) {}

    /**\brief The destructor*/
  virtual ~PkgScopeBase() {}

  public:
    PackageId packageIdOfVarId(VarId varId) const;
    std::string getDesignation(const IdPkgRel& r) const;
    std::string getVersion(VarId varId) const;
    void fillPkgData(VarId varId, Pkg& pkg) const;
    std::string constructPackageName(VarId varId) const;
    std::string getPackageName(VarId varId) const;
    std::string constructPackageNameWithBuildTime(VarId varId) const;
    bool checkName(const std::string& name) const;
    PackageId strToPackageId(const std::string& name) const;
    std::string packageIdToStr(PackageId packageId) const;

protected:
    int versionCompare(const std::string& ver1, const std::string& ver2) const;
    bool versionOverlap(const VersionCond& ver1, const VersionCond& ver2) const;
    bool versionEqual(const std::string& ver1, const std::string& ver2) const;
    bool versionGreater(const std::string& ver1, const std::string& ver2) const;
    std::string constructFullVersion(VarId varId) const;

protected:
    const AbstractPackageBackEnd& m_backEnd;
    const Snapshot& m_snapshot;
    const SnapshotPkgVector& m_pkgs;
    const SnapshotRelationVector& m_relations;
    const ProvideMap& m_provideMap;
    const InstalledReferences& m_installedRequiresEntries;
    const InstalledReferences& m_installedConflictsEntries;
  }; //class PkgScopeBase;
} //namespace Deepsolver;

#endif //DEEPSOLVER_Pkg_SCOPE_BASE_H;
