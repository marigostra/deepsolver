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
#include"deepsolver/PkgUtils.h"

DEEPSOLVER_BEGIN_NAMESPACE

void PkgUtils::fillWithhInstalledPackages(AbstractPackageBackEnd& backEnd,
					  PkgSnapshot::Snapshot& snapshot,
					  ConstCharVector& strings,
					  bool stopOnInvalidPkg)
{
  PkgSnapshot::PkgVector& pkgs = snapshot.pkgs;
  std::auto_ptr<AbstractInstalledPackagesIterator> it = backEnd.enumInstalledPackages();
  size_t installedCount = 0;
  PkgVector toInhanceWith;
  Pkg pkg;
  while(it->moveNext(pkg))
    {
      if (!pkg.valid())
	{
	  if (stopOnInvalidPkg)
	    throw OperationException(OperationException::InvalidInstalledPkg); else //FIXME:Add package designation here;
	    logMsg(LOG_WARNING, "OS has an invalid package: %s", pkg.designation().c_str());
	}
      installedCount++;
      const PkgId pkgId = PkgSnapshot::strToPkgId(snapshot, pkg.name);//FIXME:must be got with checkName();
      if (pkgId == BadPkgId)
	{
	  toInhanceWith.push_back(pkg);
	  continue;
	}
      VarId fromVarId, toVarId;
      PkgSnapshot::locateRange(snapshot, pkgId, fromVarId, toVarId);
      //Here fromVarId can be equal to toVarId. That means name of installed package is met in relations of attached repositories;
      bool found = 0;
      for(VarId varId = fromVarId;varId < toVarId;varId++)
	{
	  assert(varId < pkgs.size());
	  PkgSnapshot::Pkg& oldPkg = pkgs[varId];
	  assert(oldPkg.pkgId == pkgId);
	  if (PkgSnapshot::theSameVersion(pkg, oldPkg))
	    {
	      oldPkg.flags |= PkgFlagInstalled;
	      found = 1;
	    }
	}
      if (!found)
	toInhanceWith.push_back(pkg);
    } //while(installed packages);
  logMsg(LOG_DEBUG, "utils:the system has %zu installed packages, %zu of them should be added to the existing snapshot", installedCount, toInhanceWith.size());
  PkgSnapshot::enhance(snapshot, toInhanceWith, PkgFlagInstalled, strings);
}

void PkgUtils::prepareReversedMaps(const PkgSnapshot::Snapshot& snapshot,
			 ProvideMap& provideMap,
			 InstalledReferences& requires,
			 InstalledReferences& conflicts)
{
  const clock_t providesStart = clock();
  provideMap.fillWith(snapshot);
  const double providesDuration = ((double)clock() - providesStart) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "Auxiliary provides map construction takes %f sec", providesDuration);
  const PkgSnapshot::PkgVector& pkgs = snapshot.pkgs;
  const PkgSnapshot::RelationVector& rels = snapshot.relations;
  const clock_t installedStart = clock();
  for(PkgSnapshot::PkgVector::size_type i = 0;i < pkgs.size();i++)
    if (pkgs[i].flags & PkgFlagInstalled)
      {
	const PkgSnapshot::Pkg& pkg = pkgs[i];
	size_t pos = pkg.requiresPos, count = pkg.requiresCount;
	for(size_t k = 0;k < count;k++)
	  {
	    assert(pos + k < rels.size());
	    requires.add(rels[pos + k].pkgId, i);
	  }
	pos = pkg.conflictsPos;
	count = pkg.conflictsCount;
	for(size_t k = 0;k < count;k++)
	  {
	    assert(pos + k < rels.size());
	    conflicts.add(rels[pos + k].pkgId, i);
	  }
      }
  requires.commit();
  conflicts.commit();
  const double installedDuration = ((double)clock() - installedStart) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "Installed package requires/conflicts reversed map construction takes %f sec", installedDuration);
}

void PkgUtils::fillUpgradeDowngrade(const AbstractPackageBackEnd& backEnd,
				    const AbstractPackageScope& scope,
				    VarIdVector& install,
				    VarIdVector& remove,
				    VarIdToVarIdMap& upgrade,
				    VarIdToVarIdMap& downgrade)
{
  upgrade.clear();
  downgrade.clear();
  for(VarIdVector::size_type i = 0;i < install.size();i++)
    {
      if (install[i] == BadVarId)
	continue;
      VarIdVector::size_type j;
      for(j = 0;j < remove.size();j++)
	if (remove[j] != BadVarId && scope.packageIdOfVarId(remove[j]) == scope.packageIdOfVarId(install[i]))
	  break;
      if (j >= remove.size())
	continue;
      const std::string versionToInstall = scope.getVersion(install[i]);
      const std::string versionToRemove = scope.getVersion(remove[j]);
      if (!backEnd.versionEqual(versionToInstall, versionToRemove))//Packages may be  with equal versions but with different build times;
    {
      if (backEnd.versionGreater(versionToInstall, versionToRemove))
	upgrade.insert(VarIdToVarIdMap::value_type(remove[j], install[i])); else
	downgrade.insert(VarIdToVarIdMap::value_type(remove[j], install[i]));
      install[i] = BadVarId;
    } else
	logMsg(LOG_WARNING, "upgrade:trying to upgrade packages with same version but with different build time: %s and %s", scope.constructPackageNameWithBuildTime(install[i]).c_str(), scope.constructPackageNameWithBuildTime(remove[j]).c_str());
      remove[j] = BadVarId;
    }
  for(VarIdVector::size_type i = 0;i < install.size();i++)
    while (i < install.size() && install[i] == BadVarId)
      {
	install[i] = install.back();
	install.pop_back();
      }
  for(VarIdVector::size_type i = 0;i < remove.size();i++)
    while (i < remove.size() && remove[i] == BadVarId)
      {
	remove[i] = remove.back();
	remove.pop_back();
      }
}


DEEPSOLVER_END_NAMESPACE
