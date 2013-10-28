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
#include"deepsolver/PkgScopeBase.h"

DEEPSOLVER_BEGIN_NAMESPACE

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
  const std::string name = getPkgName(varId);
  const std::string ver = getVersion(varId, epochMode);
  assert(!name.empty() && !ver.empty());
  return name + "-" + ver;
}

std::string PkgScopeBase::getDesignation(const IdPkgRel& r) const
{
  assert(r.pkgId != BadPkgId);
  std::string s = pkgIdToStr(r.pkgId);
  if (r.verDir == VerNone)
    return s;
  assert(!r.ver.empty());
  s += " ";
  if (r.verDir & VerLess)
    s += "<";
  if (r.verDir & VerGreater)
    s += ">";
  if (r.verDir & VerEquals)
    s += "=";
  return s + " " + r.ver;
}

std::string PkgScopeBase::getVersion(VarId varId, int epochMode) const
{
  assert(varId != BadVarId && varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  assert(pkg.ver != NULL && pkg.release != NULL);
  std::ostringstream res;
  switch(epochMode)
    {
    case EpochAlways:
      res << pkg.epoch << ":";
      break;
    case EpochIfNonZero:
  res << pkg.epoch << ":";
  break;
    };
  res << pkg.ver << "-" << pkg.release;
  return res.str();
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

bool PkgScopeBase::verOverlap(const VersionCond& ver1, const VersionCond& ver2) const
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

DEEPSOLVER_END_NAMESPACE
