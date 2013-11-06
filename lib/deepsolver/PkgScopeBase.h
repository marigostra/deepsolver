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

#include"deepsolver/AbstractPkgBackEnd.h"
#include"deepsolver/SolverBase.h"
#include"deepsolver/PkgSnapshot.h"

namespace Deepsolver
{
  class PkgScopeBase: public AbstractPkgScope
  {
  protected:
    typedef PkgSnapshot::Snapshot Snapshot;
    typedef PkgSnapshot::Pkg SnapshotPkg;
    typedef PkgSnapshot::PkgVector SnapshotPkgVector;
    typedef PkgSnapshot::Relation SnapshotRelation;
    typedef PkgSnapshot::RelationVector SnapshotRelationVector;

  public:
    PkgScopeBase(const AbstractPkgBackEnd& backend, const Snapshot& snapshot)
      : m_backend(backend),
	m_snapshot(snapshot),
	m_pkgs(snapshot.pkgs),
	m_relations(snapshot.relations) {}

    /**\brief The destructor*/
  virtual ~PkgScopeBase() {}

  public:
    size_t getPkgCount() const override;
    bool knownPkgName(const std::string& name) const override;
    std::string getPkgName(VarId varId) const override;
    std::string getDesignation(VarId varId, int epochMode) const override;
    std::string getDesignation(const IdPkgRel& r) const override;
    std::string getVersion(VarId varId, int epochMode) const override;
    void fullPkgData(VarId varId, Pkg& pkg) const override;
    PkgId pkgIdOfVarId(VarId varId) const override;
    std::string pkgIdToStr(PkgId pkgId) const override;
    PkgId strToPkgId(const std::string& name) const override;

protected:
    int verCmp(const std::string& ver1, const std::string& ver2) const;
    bool verOverlap(const VersionCond& ver1, const VersionCond& ver2) const;
    bool verEqual(const std::string& ver1, const std::string& ver2) const;
    bool verGreater(const std::string& ver1, const std::string& ver2) const;

protected:
    const AbstractPkgBackEnd& m_backend;
    const Snapshot& m_snapshot;
    const SnapshotPkgVector& m_pkgs;
    const SnapshotRelationVector& m_relations;
  }; //class PkgScopeBase;
} //namespace Deepsolver;

#endif //DEEPSOLVER_Pkg_SCOPE_BASE_H;
