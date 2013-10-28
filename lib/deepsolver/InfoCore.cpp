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
#include"deepsolver/InfoCore.h"
#include"deepsolver/AbstractPkgBackEnd.h"
#include"deepsolver/PkgSnapshot.h"

DEEPSOLVER_BEGIN_NAMESPACE

void InfoCore::listKnownPackages(PkgVector& pkgs, bool noInstalled, bool noRepoAvailable)
{
  /*FIXME:Should go to OperationCore
  assert(!noInstalled || !noRepoAvailable);
  pkgs.clear();
  if (noRepoAvailable && !noInstalled)
    {
      File::readAhead("/var/lib/rpm/Packages");//FIXME:take the value from configuration;
      logMsg(LOG_DEBUG, "Info core is requested to list all installed packages without available with known repositories");
      std::auto_ptr<AbstractPkgBackEnd> backend = createRpmBackEnd();
      std::auto_ptr<AbstractInstalledPkgIterator> it = backend->enumInstalledPkg();
      Pkg pkg;
      while(it->moveNext(pkg))
	pkgs.push_back(pkg);
      logMsg(LOG_DEBUG, "%zu packages enumerated", pkgs.size());
      return;
    }
  logMsg(LOG_DEBUG, "Info core is requested to enumerate packages available by known repositories");
  PkgSnapshot::Snapshot snapshot;
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  logMsg(LOG_DEBUG, "%Loaded information about %zu available packages", snapshot.pkgs.size());
  if (!noInstalled)
    {
      File::readAhead("/var/lib/rpm/Packages");//FIXME:take the value from configuration;
      std::auto_ptr<AbstractPkgBackEnd> backend = createRpmBackEnd();
      logMsg(LOG_DEBUG, "Adding information about installed packages");
      PkgUtils::fillWithhInstalledPackages(*backend.get(), snapshot, m_autoReleaseStrings, m_conf.root().stopOnInvalidInstalledPkg);
    }
  const PkgSnapshot::PkgVector& p = snapshot.pkgs;
  pkgs.resize(p.size());
  for(PkgSnapshot::PkgVector::size_type i = 0;i < p.size();i++)
    {
      assert(p[i].ver != NULL && p[i].release != NULL);
      pkgs[i].name = PkgSnapshot::pkgIdToStr(snapshot, p[i].pkgId);
      pkgs[i].epoch = p[i].epoch;
      pkgs[i].version = p[i].ver;
      pkgs[i].release = p[i].release;
      pkgs[i].buildTime = p[i].buildTime;
    }
  logMsg(LOG_DEBUG, "Collected list of %zu packages", pkgs.size());
  */
}

DEEPSOLVER_END_NAMESPACE
