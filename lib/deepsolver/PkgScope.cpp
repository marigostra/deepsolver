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
#include"deepsolver/PkgScope.h"

#define HAS_VERSION(x) ((x).ver != NULL)

DEEPSOLVER_BEGIN_NAMESPACE

namespace
{
  VerSubset constructVerSubsetEquals(int epoch, 
						   const std::string& version,
						   const std::string& release)
  {
    assert(!version.empty());
    assert(!release.empty());
    std::ostringstream ss;
    ss  << epoch << ":" << version << "-" << release;
    return VerSubset(ss.str(), VerEquals);
  }
}

void PkgScope::selectMatchingVarsProvidesOnly(const IdPkgRel& rel, VarIdVector& vars) const
{
  vars.clear();
  if (rel.verRestricted())
    selectMatchingVarsProvidesOnly(rel.pkgId, rel.extractVerSubset(), vars); else
    selectMatchingVarsProvidesOnly(rel.pkgId, vars);
}

  //Only provides must be considered here;
void PkgScope::selectMatchingVarsProvidesOnly(PkgId pkgId, VarIdVector& vars) const
{
  vars.clear();
  selectVarsToTry(pkgId, vars, 0);//0 means do not include packageId itself;
}

void PkgScope::selectMatchingVarsProvidesOnly(PackageId packageId, const VerSubset& ver, VarIdVector& vars) const
{
  //Considering only provides entries and only with version information;
  vars.clear();
  VarIdVector toTry;
  selectVarsToTry(packageId, toTry, 0);//0 means do not include packageId itself;
  for(VarIdVector::size_type i = 0;i < toTry.size();i++)
    {
      assert(toTry[i] < m_pkgs.size());
      const size_t pos = m_pkgs[toTry[i]].providesPos;
      const size_t count = m_pkgs[toTry[i]].providesCount;
      assert(count > 0);
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < m_relations.size());
	  if (!HAS_VERSION(m_relations[pos + j]))
	    continue;  
	  assert(m_relations[pos + j].verDir != VerNone);
	  if (m_relations[pos + j].pkgId == packageId && 
	      verOverlap(VerSubset(m_relations[pos + j].ver, m_relations[pos + j].verDir), ver))
	    break;
	}
      if (j < count)
	vars.push_back(toTry[i]);
    }
}

void PkgScope::selectMatchingVarsRealNames(const IdPkgRel& rel, VarIdVector& vars) const
{
  vars.clear();
  if (rel.verRestricted())
    selectMatchingVarsRealNames(rel.pkgId, rel.extractVerSubset(), vars); else
    selectMatchingVarsRealNames(rel.pkgId, vars);
}

void PkgScope::selectMatchingVarsRealNames(PackageId packageId, VarIdVector& vars) const
{
  //Here we must process only real package names, no provides are required;
  vars.clear();
  VarId fromPos, toPos;
  PkgSnapshot::locateRange(m_snapshot, packageId, fromPos, toPos);
  assert(fromPos < m_pkgs.size() && toPos < m_pkgs.size());
  for(VarId i = fromPos;i < toPos;i++)
    {
      assert(m_pkgs[i].pkgId == packageId);
      vars.push_back(i);
    }
}

void PkgScope::selectMatchingVarsRealNames(PackageId packageId, const VerSubset& ver, VarIdVector& vars) const
{
  //Here we must process only real package names, no provides are required;
  vars.clear();
  VarId fromPos, toPos;
  PkgSnapshot::locateRange(m_snapshot, packageId, fromPos, toPos);
  assert(fromPos < m_pkgs.size() && toPos < m_pkgs.size());
  for(VarId i = fromPos;i < toPos;i++)
    {
      assert(m_pkgs[i].pkgId == packageId);
      if (verOverlap(constructVerSubsetEquals(m_pkgs[i].epoch, m_pkgs[i].ver, m_pkgs[i].release), ver))
	vars.push_back(i);
    }
}

void PkgScope::selectMatchingVarsWithProvides(const IdPkgRel& rel, VarIdVector& vars) const
{
  vars.clear();
  if (rel.verRestricted())
    selectMatchingVarsWithProvides(rel.pkgId, rel.extractVerSubset(), vars); else
    selectMatchingVarsWithProvides(rel.pkgId, vars);
}

void PkgScope::selectMatchingVarsWithProvides(PackageId pkgId, VarIdVector& vars) const
{
  vars.clear();
  selectVarsToTry(pkgId, vars, 1);//1 means include packageId itself;
}

void PkgScope::selectMatchingVarsWithProvides(PackageId packageId, const VerSubset& ver, VarIdVector& vars) const
{
  vars.clear();
  VarIdVector toTry;
  selectVarsToTry(packageId, toTry, 1);//1 means include packageId itself;
  for(VarIdVector::size_type i = 0;i < toTry.size();i++)
    {
      assert(toTry[i] < m_pkgs.size());
      if (m_pkgs[toTry[i]].pkgId == packageId && verOverlap(constructVerSubsetEquals(m_pkgs[toTry[i]].epoch, m_pkgs[toTry[i]].ver, m_pkgs[toTry[i]].release), ver))
	{
	  vars.push_back(toTry[i]);
	  continue;
	}
      const size_t pos = m_pkgs[toTry[i]].providesPos;
      const size_t count = m_pkgs[toTry[i]].providesCount;
      if (count == 0)//There are no provides entries;
	continue;
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < m_relations.size());
	  if (!HAS_VERSION(m_relations[pos + j]))
	    continue;  
	  assert(m_relations[pos + j].verDir != VerNone);
	  if (m_relations[pos + j].pkgId == packageId && verOverlap(VerSubset(m_relations[pos + j].ver, m_relations[pos + j].verDir), ver))
	    break;
	}
      if (j < count)
	vars.push_back(toTry[i]);
    }
}

bool PkgScope::isInstalled(VarId varId) const
{
  assert(varId != BadVarId);
  assert(varId < m_pkgs.size());
  return m_pkgs[varId].flags & PkgFlagInstalled;
}

void PkgScope::selectTheNewest(VarIdVector& vars) const
{
  //Processing only real version of provided packages;
  if (vars.size() < 2)
    return;
  VarId currentMax = vars[0];
  assert(currentMax < m_pkgs.size());
  assert(m_pkgs[currentMax].ver != NULL);
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < m_pkgs.size());
  assert(m_pkgs[vars[i]].ver != NULL);
  if (verGreater(getVersion(vars[i], EpochAlways), getVersion(currentMax, EpochAlways)))
    currentMax = vars[i];
    }
  assert(currentMax < m_pkgs.size());
  const std::string maxVersion = getVersion(currentMax, EpochAlways);
  size_t hasCount = 0;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    if (verEqual(getVersion(vars[i], EpochAlways), maxVersion))
      vars[hasCount++] = vars[i];
  assert(hasCount > 0);
  vars.resize(hasCount);
}

void PkgScope::selectTheNewestByProvide(VarIdVector& vars, PackageId provideEntry) const
{
  if (vars.size() < 2)
    return;
  StringVector versions;
  versions.resize(vars.size());
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < m_pkgs.size());
      const SnapshotPkg& pkg = m_pkgs[vars[i]];
      const size_t pos = pkg.providesPos;
      const size_t count = pkg.providesCount;
      assert(pos > 0 && count > 0);//It means the package has any provides;
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < m_relations.size());
	  if (m_relations[pos + j].pkgId == provideEntry)
	    break;
	}
      assert(j < count);//The package contains needed provide entry;
      assert(HAS_VERSION(m_relations[pos + j]));
      assert(m_relations[pos + j].verDir == VerEquals);//It is very strict constraint based on ALT Linux policy, but it would be better if it is so;
      versions[i] = m_relations[pos + j].ver;
    }
  assert(vars.size() == versions.size());
  size_t currentMax = 0;
  for(StringVector::size_type i = 0;i < versions.size();i++)
    if (verGreater(versions[i], versions[currentMax]))
      currentMax = i;
  const std::string maxVersion = versions[currentMax];
  size_t hasCount = 0;
  for(StringVector::size_type i = 0;i < versions.size();i++)
    if (verEqual(versions[i], maxVersion))
      vars[hasCount++] = vars[i];
  assert(hasCount > 0);
  vars.resize(hasCount);
}

bool PkgScope::allProvidesHaveTheVersion(const VarIdVector& vars, PackageId provideEntry) const
{
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < m_pkgs.size());
      const SnapshotPkg& pkg = m_pkgs[vars[i]];
      const size_t pos = pkg.providesPos;
      const size_t count = pkg.providesCount;
      assert(count > 0);
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < m_relations.size());
	  if (m_relations[pos + j].pkgId == provideEntry)
	    break;
	}
      assert(j < count);
      if (!HAS_VERSION(m_relations[pos + j]))
	return 0;
    }
  return 1;
}

void PkgScope::getRequires(VarId varId, IdPkgRelVector& res) const
{
  assert(varId != BadVarId);
  PackageIdVector withVersion, withoutVersion;
  VerSubsetVector versions;
  getRequires(varId, withoutVersion, withVersion, versions);
  assert(withVersion.size() == versions.size());
  res.clear();
  for(PackageIdVector::size_type i = 0;i < withoutVersion.size();i++)
    res.push_back(IdPkgRel(withoutVersion[i]));
  for(PackageIdVector::size_type i = 0;i < withVersion.size();i++)
    res.push_back(IdPkgRel(withVersion[i], versions[i]));
}

void PkgScope::getConflicts(VarId varId, IdPkgRelVector& res) const
{
  assert(varId != BadVarId);
  PackageIdVector withVersion, withoutVersion;
  VerSubsetVector versions;
  getConflicts(varId, withoutVersion, withVersion, versions);
  assert(withVersion.size() == versions.size());
  res.clear();
  for(PackageIdVector::size_type i = 0;i < withoutVersion.size();i++)
    res.push_back(IdPkgRel(withoutVersion[i]));
  for(PackageIdVector::size_type i = 0;i < withVersion.size();i++)
    res.push_back(IdPkgRel(withVersion[i], versions[i]));
}

void PkgScope::whatSatisfyAmongInstalled(const IdPkgRel& rel, VarIdVector& res) const
{
  assert(rel.pkgId != BadPkgId);
  res.clear();

  //If there is no version restrictions;
  if (!rel.verRestricted())
    {
      VarIdVector vars;
      selectVarsToTry(rel.pkgId, vars, 1);//1 means to include package itself;
      for(VarIdVector::size_type i = 0;i < vars.size();i++)
	{
	  assert(vars[i] < m_pkgs.size());
	  if (m_pkgs[vars[i]].flags & PkgFlagInstalled)
	    res.push_back(vars[i]);
	}
      return;
    } //without version;

  //If there is version restriction;
  VarIdVector vars;
  selectVarsToTry(rel.pkgId, vars, 1);//1 means to include package itself;
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      assert(vars[i] < m_pkgs.size());
      const SnapshotPkg& pkg = m_pkgs[vars[i]];
      if (!(pkg.flags & PkgFlagInstalled))
	continue;
      if (pkg.pkgId == rel.pkgId &&
	  verOverlap(constructVerSubsetEquals(pkg.epoch, pkg.ver, pkg.release), VerSubset(rel.ver, rel.verDir)))
	{
	  res.push_back(vars[i]);
	  continue;
	}
      const size_t pos = pkg.providesPos;
      const size_t count = pkg.providesCount;
      if (count == 0)//There are no provides entries;
	continue;
      size_t j;
      for(j = 0;j < count;j++)
	{
	  assert(pos + j < m_relations.size());
	  if (m_relations[pos + j].pkgId != rel.pkgId ||
	      !HAS_VERSION(m_relations[pos + j]))//Provide entry has no version;
	    continue;
	  assert(m_relations[pos + j].verDir != VerNone);
	  if (verOverlap(VerSubset(m_relations[pos + j].ver, m_relations[pos + j].verDir), VerSubset(rel.ver, rel.verDir)))
	    {
	      res.push_back(vars[i]);
	      break;
	    }
	} //for(provides);
    } //for(packages);
}

void PkgScope::whatDependAmongInstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels) const
{
  res.clear();
  resRels.clear();
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  VarIdVector v;
  //First of all checking the package itself;
  findPkgsByRequire(m_pkgs[varId].pkgId, v);
  for(VarIdVector::size_type i = 0;i < v.size();i++)
    {
      assert(v[i] < m_pkgs.size());
      assert(m_pkgs[v[i]].flags & PkgFlagInstalled);
      PackageIdVector withoutVersion, withVersion;
      VerSubsetVector versions;
      getRequires(v[i], withoutVersion, withVersion, versions);
      assert(withVersion.size() == versions.size());
      for(PackageIdVector::size_type k = 0;k < withoutVersion.size();k++)
	if (withoutVersion[k] == pkg.pkgId)
	  {
	    res.push_back(v[i]);
	    resRels.push_back(IdPkgRel(withoutVersion[k]));
	  }
      for(PackageIdVector::size_type k = 0;k < withVersion.size();k++)
	if (withVersion[k] == pkg.pkgId && verOverlap(constructVerSubsetEquals(pkg.epoch, pkg.ver, pkg.release), versions[k]))
	  {
	    res.push_back(v[i]);
	    resRels.push_back(IdPkgRel(withVersion[k], versions[k]));
	  }
    } //For every package depending on varId without provides;
  //Now we check all provide entries of pkg;
  const size_t pos = pkg.providesPos;
  const size_t count = pkg.providesCount;
  for(SnapshotRelationVector::size_type i = 0;i < count;i++)
    {
      assert(pos + i < m_relations.size());
      const SnapshotRelation& rel = m_relations[pos + i];
      v.clear();
      findPkgsByRequire(rel.pkgId, v);
      for(VarIdVector::size_type k = 0;k < v.size();k++)
	{
	  assert(v[k] < m_pkgs.size());
	  assert(m_pkgs[v[k]].flags & PkgFlagInstalled);
	  PackageIdVector withoutVersion, withVersion;
	  VerSubsetVector versions;
	  getRequires(v[k], withoutVersion, withVersion, versions);
	  assert(withVersion.size() == versions.size());
	  //Checking without version anyway;
	  for(PackageIdVector::size_type q = 0;q < withoutVersion.size();q++)
	    if (withoutVersion[q] == rel.pkgId)
	      {
		res.push_back(v[k]);
		resRels.push_back(IdPkgRel(withoutVersion[q]));
	      }
	  //With version must be checked only if provide entry has the version
	  if (HAS_VERSION(rel))
	    {
	      assert(rel.verDir == VerEquals);//Actually it shouldn't be an assert, we can silently skip this provide;
	      for(PackageIdVector::size_type q = 0;q < withVersion.size();q++)
		if (withVersion[q] == rel.pkgId && verOverlap(VerSubset(rel.ver, VerEquals), versions[q]))
		  {
		    res.push_back(v[k]);
		    resRels.push_back(IdPkgRel(withVersion[q], versions[q]));
		  }
	    }//if has version;
	}
    } //for provides;
}

void PkgScope::whatConflictAmongInstalled(VarId varId, VarIdVector& res, IdPkgRelVector& resRels) const
{
  res.clear();
  resRels.clear();
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  VarIdVector v;
  //First of all checking the package itself;
findPkgsByConflict(m_pkgs[varId].pkgId, v);
  for(VarIdVector::size_type i = 0;i < v.size();i++)
    {
      assert(v[i] < m_pkgs.size());
      assert(m_pkgs[v[i]].flags & PkgFlagInstalled);
      PackageIdVector withoutVersion, withVersion;
      VerSubsetVector versions;
      getConflicts(v[i], withoutVersion, withVersion, versions);
      assert(withVersion.size() == versions.size());
      for(PackageIdVector::size_type k = 0;k < withoutVersion.size();k++)
	if (withoutVersion[k] == pkg.pkgId)
	  {
	    res.push_back(v[i]);
	    resRels.push_back(IdPkgRel(withoutVersion[k]));
	  }
      for(PackageIdVector::size_type k = 0;k < withVersion.size();k++)
	if (withVersion[k] == pkg.pkgId && verOverlap(constructVerSubsetEquals(pkg.epoch, pkg.ver, pkg.release), versions[k]))
	  {
	    res.push_back(v[i]);
	    resRels.push_back(IdPkgRel(withVersion[k], versions[k]));
	  }
    } //For every package depending on varId without provides;
  //Now we check all provide entries of pkg;
  const size_t pos = pkg.providesPos;
  const size_t count = pkg.providesCount;
  for(SnapshotRelationVector::size_type i = 0;i < count;i++)
    {
      assert(pos + i < m_relations.size());
      const SnapshotRelation& rel = m_relations[pos + i];
      v.clear();
findPkgsByConflict(rel.pkgId, v);
      for(VarIdVector::size_type k = 0;k < v.size();k++)
	{
	  assert(v[k] < m_pkgs.size());
	  assert(m_pkgs[v[k]].flags & PkgFlagInstalled);
	  PackageIdVector withoutVersion, withVersion;
	  VerSubsetVector versions;
	  getConflicts(v[k], withoutVersion, withVersion, versions);
	  assert(withVersion.size() == versions.size());
	  //Checking without version anyway;
	  for(PackageIdVector::size_type q = 0;q < withoutVersion.size();q++)
	    if (withoutVersion[q] == rel.pkgId)
	      {
		res.push_back(v[k]);
		resRels.push_back(IdPkgRel(withoutVersion[q]));
	      }
	  //With version must be checked only if provide entry has the version
	  if (HAS_VERSION(rel))
	    {
	      assert(rel.verDir == VerEquals);//FIXME:Actually it shouldn't be an assert, we can silently skip this provide;
	      for(PackageIdVector::size_type q = 0;q < withVersion.size();q++)
		if (withVersion[q] == rel.pkgId && verOverlap(VerSubset(rel.ver, VerEquals), versions[q]))
		  {
		    res.push_back(v[k]);
		    resRels.push_back(IdPkgRel(withVersion[q], versions[q]));
		  }
	    }//if has version;
	}
    } //for provides;
}

// Private methods;

void PkgScope::getRequires(VarId varId,
			       PackageIdVector& depWithoutVersion,
			       PackageIdVector& depWithVersion,
			       VerSubsetVector& versions) const
{
  depWithoutVersion.clear();
  depWithVersion.clear();
  versions.clear();
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  const size_t pos = pkg.requiresPos;
  const size_t count = pkg.requiresCount;
  for(size_t i = 0;i < count;i++)
    {
      assert(pos + i < m_relations.size());
      if (!HAS_VERSION(m_relations[pos + i]))
	depWithoutVersion.push_back(m_relations[pos + i].pkgId); else 
	{
	  depWithVersion.push_back(m_relations[pos + i].pkgId);
	  versions.push_back(VerSubset(m_relations[pos + i].ver, m_relations[pos + i].verDir));
	}
    }
}

void PkgScope::getConflicts(VarId varId,
				PackageIdVector& withoutVersion,
				PackageIdVector& withVersion,
				VerSubsetVector& versions) const
{
  withoutVersion.clear();
  withVersion.clear();
  versions.clear();
  assert(varId < m_pkgs.size());
  const SnapshotPkg& pkg = m_pkgs[varId];
  const size_t pos = pkg.conflictsPos;
  const size_t count = pkg.conflictsCount;
  for(size_t i = 0;i < count;i++)
    {
      assert(pos + i < m_relations.size());
      if (m_relations[pos + i].ver == NULL)
	withoutVersion.push_back(m_relations[pos + i].pkgId); else 
	{
	  withVersion.push_back(m_relations[pos + i].pkgId);
	  versions.push_back(VerSubset(m_relations[pos + i].ver, m_relations[pos + i].verDir));
	}
    }
}

void PkgScope::selectVarsToTry(PkgId pkgId,
			    VarIdVector& toTry,
			       bool includeItself) const
{
  toTry.clear();
  if (includeItself)
    {
      VarId fromPos, toPos;
      PkgSnapshot::locateRange(m_snapshot, pkgId, fromPos, toPos);
      assert(fromPos < m_pkgs.size() && toPos < m_pkgs.size());
      for(VarId i = fromPos;i < toPos;i++)
	toTry.push_back(i);
    }
  VarIdVector providers;
findProviders(pkgId, providers);//FIXME:Good idea to push to toTry vector directly;
  for(VarIdVector::size_type i = 0;i < providers.size();i++)
    toTry.push_back(providers[i]);
  //Maybe it is good idea to perform doubling cleaning here, but it will take time;
}

DEEPSOLVER_END_NAMESPACE
