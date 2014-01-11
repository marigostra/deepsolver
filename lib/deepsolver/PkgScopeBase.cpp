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
#include"deepsolver/PkgScopeBase.h"

DEEPSOLVER_BEGIN_NAMESPACE

size_t PkgScopeBase::getPkgCount() const
{
  return m_pkgs.size();
}

bool PkgScopeBase::knownPkgName(const std::string& name) const
{
  assert(!name.empty());
  return PkgSnapshot::checkName(m_snapshot, name);
}

std::string PkgScopeBase::getPkgName(VarId varId) const
{
  assert(varId != BadVarId && varId < m_pkgs.size());
  return pkgIdToStr(m_pkgs[varId].pkgId);
}

std::string PkgScopeBase::getDesignation(VarId varId, int epochMode) const
{
  assert(varId != BadVarId);
  return m_backend.combineNameAndVer(getPkgName(varId), getVersion(varId, epochMode));
}

std::string PkgScopeBase::getDesignation(const IdPkgRel& r) const
{
  assert(r.pkgId != BadPkgId);
  NamedPkgRel namedRel(pkgIdToStr(r.pkgId), r.verDir, r.ver);
  return m_backend.getDesignation(namedRel);
}

std::string PkgScopeBase::getVersion(VarId varId, int epochMode) const
{
  assert(varId != BadVarId && varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  assert(pkg.ver != NULL && pkg.release != NULL);
  return m_backend.makeVer(pkg.epoch, pkg.ver, pkg.release, epochMode);
}

void PkgScopeBase::fullPkgData(VarId varId, Pkg& pkg) const
{
  assert(varId != BadVarId && varId < m_pkgs.size());
  const SnapshotPkg& p = m_pkgs[varId];
  assert(p.ver != NULL || p.release != NULL);
  pkg.name = pkgIdToStr(p.pkgId);
  pkg.epoch = p.epoch;
  pkg.version = p.ver;
  pkg.release = p.release;
  pkg.buildTime = p.buildTime;
  for(size_t i = p.providesPos;i < p.providesPos + p.providesCount;++i)
    {
      assert(i < m_relations.size());
      pkg.provides.push_back(makeNamedPkgRel(m_relations[i]));
    }
  for(size_t i = p.requiresPos;i < p.requiresPos + p.requiresCount;++i)
    {
      assert(i < m_relations.size());
      pkg.requires.push_back(makeNamedPkgRel(m_relations[i]));
    }
  for(size_t i = p.conflictsPos;i < p.conflictsPos + p.conflictsCount;++i)
    {
      assert(i < m_relations.size());
      pkg.conflicts.push_back(makeNamedPkgRel(m_relations[i]));
    }
  for(size_t i = p.obsoletesPos;i < p.obsoletesPos + p.obsoletesCount;++i)
    {
      assert(i < m_relations.size());
      pkg.obsoletes.push_back(makeNamedPkgRel(m_relations[i]));
    }
}

PkgId PkgScopeBase::pkgIdOfVarId(VarId varId) const
{
  assert(varId != BadVarId && varId < m_pkgs.size());
  return m_pkgs[varId].pkgId;
}

std::string PkgScopeBase::pkgIdToStr(PkgId pkgId) const
{
  assert(pkgId != BadPkgId);
  return PkgSnapshot::pkgIdToStr(m_snapshot, pkgId);
}

PkgId PkgScopeBase::strToPkgId(const std::string& name) const
{
  assert(!name.empty());
  return PkgSnapshot::strToPkgId(m_snapshot, name);
}

int PkgScopeBase::verCmp(const std::string& ver1, const std::string& ver2) const
{
  assert(!ver1.empty() && !ver2.empty());
  return m_backend.verCmp(ver1, ver2);
}

bool PkgScopeBase::verOverlap(const VerSubset& ver1, const VerSubset& ver2) const
{
  //FIXME:  assert(ver1.valid() && !ver2.valid());
  return m_backend.verOverlap(ver1, ver2);
}

bool PkgScopeBase::verEqual(const std::string& ver1, const std::string& ver2) const
{
  assert(!ver1.empty() && !ver2.empty());
  return m_backend.verEqual(ver1, ver2);
}

bool PkgScopeBase::verGreater(const std::string& ver1, const std::string& ver2) const
{
  assert(!ver1.empty() && !ver2.empty());
  return m_backend.verGreater(ver1, ver2);
}

NamedPkgRel PkgScopeBase::makeNamedPkgRel(const SnapshotRelation& rel) const
{
  assert(rel.pkgId != BadPkgId);
  assert(rel.pkgId < m_snapshot.pkgNames.size());
  assert(rel.verDir == VerNone || rel.ver != NULL);
  if (rel.verDir == VerNone)
    return NamedPkgRel(pkgIdToStr(rel.pkgId));
  return NamedPkgRel(pkgIdToStr(rel.pkgId), rel.verDir, rel.ver);
}

DEEPSOLVER_END_NAMESPACE
