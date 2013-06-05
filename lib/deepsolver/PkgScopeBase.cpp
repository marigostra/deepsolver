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

PkgId PkgScopeBase::packageIdOfVarId(VarId varId) const
{
  assert(varId < m_pkgs.size());
  return m_pkgs[varId].pkgId;
}

std::string PkgScopeBase::getVersion(VarId varId) const
{
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  assert(pkg.ver != NULL && pkg.release != NULL);
  std::ostringstream res;
  res << pkg.epoch << ":" << pkg.ver << "-" << pkg.release;
  return res.str();
}

void PkgScopeBase::fillPkgData(VarId varId, Pkg& dest) const
{
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  assert(pkg.ver != NULL || pkg.release != NULL);
  dest.name = packageIdToStr(pkg.pkgId);
  dest.epoch = pkg.epoch;
  dest.version = pkg.ver;
  dest.release = pkg.release;
  dest.buildTime = pkg.buildTime;
}

std::string PkgScopeBase::constructPackageName(VarId varId) const
{
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  std::string res = packageIdToStr(pkg.pkgId);
  assert(pkg.ver != NULL);
  res += "-";
  res += pkg.ver;
  assert(pkg.release != NULL);
  res += "-";
  res += pkg.release;
  return res;
}

std::string PkgScopeBase::getPackageName(VarId varId) const
{
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
return packageIdToStr(pkg.pkgId);
}

std::string PkgScopeBase::constructPackageNameWithBuildTime(VarId varId) const
{
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  std::ostringstream res;
  res << packageIdToStr(pkg.pkgId);
  assert(pkg.ver != NULL && pkg.release != NULL);
  res << "-" << pkg.ver << "-" << pkg.release;
  res << " (" << pkg.buildTime << ")";
  return res.str();
}

bool PkgScopeBase::checkName(const std::string& name) const
{
  return PkgSnapshot::checkName(m_snapshot, name);
}

PkgId PkgScopeBase::strToPackageId(const std::string& name) const
{
  return PkgSnapshot::strToPkgId(m_snapshot, name);
}

std::string PkgScopeBase::packageIdToStr(PkgId pkgId) const
{
  return PkgSnapshot::pkgIdToStr(m_snapshot, pkgId);
}

int PkgScopeBase::versionCompare(const std::string& ver1, const std::string& ver2) const
{
  return m_backEnd.versionCompare(ver1, ver2);
}

bool PkgScopeBase::versionOverlap(const VersionCond& ver1, const VersionCond& ver2) const
{
  return m_backEnd.versionOverlap(ver1, ver2);
}

bool PkgScopeBase::versionEqual(const std::string& ver1, const std::string& ver2) const
{
  return m_backEnd.versionEqual(ver1, ver2);
}

bool PkgScopeBase::versionGreater(const std::string& ver1, const std::string& ver2) const
{
  return m_backEnd.versionGreater(ver1, ver2);
}

std::string PkgScopeBase::constructFullVersion(VarId varId) const
{
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  assert(pkg.ver != NULL && pkg.release != NULL);
  std::ostringstream ss;
  if (pkg.epoch > 0)
    ss << pkg.epoch << ":";
  ss << pkg.ver << "-" << pkg.release;
  return ss.str();
}

DEEPSOLVER_END_NAMESPACE
