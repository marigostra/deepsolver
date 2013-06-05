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
#include"deepsolver/RpmBackEnd.h"
#include"deepsolver/RpmFileHeaderReader.h"
#include"deepsolver/RpmTransaction.h"

DEEPSOLVER_BEGIN_NAMESPACE

static bool alreadyReadConfigFiles = 0;
static int buildSenseFlags(const VersionCond& c);

void RpmBackEnd::initialize()
{
  if (!alreadyReadConfigFiles)
    {
      rpmReadConfigFiles( NULL, NULL );                                                                                                                          
      alreadyReadConfigFiles = 1;
    }
}

int RpmBackEnd::versionCompare(const std::string& ver1, const std::string& ver2) const
{
  return rpmvercmp(ver1.c_str(), ver2.c_str());
}

bool RpmBackEnd::versionOverlap(const VersionCond& ver1, const VersionCond& ver2) const
{
  return rpmRangesOverlap("", ver1.version.c_str(), buildSenseFlags(ver1),
			  "", ver2.version.c_str(), buildSenseFlags(ver2));
}

bool RpmBackEnd::versionEqual(const std::string& ver1, const std::string& ver2) const
{
  return versionOverlap(VersionCond(ver1), VersionCond(ver2));
}

bool RpmBackEnd::versionGreater(const std::string& ver1, const std::string& ver2) const
{
  return versionOverlap(VersionCond(ver1, VerLess), VersionCond(ver2));
}

std::auto_ptr<AbstractInstalledPackagesIterator> RpmBackEnd::enumInstalledPackages() const
{
  std::auto_ptr<RpmInstalledPackagesIterator> rpmIterator(new RpmInstalledPackagesIterator());
  rpmIterator->openEnum();
  std::auto_ptr<AbstractInstalledPackagesIterator> it(rpmIterator.get());
  rpmIterator.release();
  return it;
}

void RpmBackEnd::readPackageFile(const std::string& fileName, PkgFile& pkgFile) const
{
  RpmFileHeaderReader reader;
  reader.load(fileName);
  reader.fillMainData(pkgFile);
  reader.fillProvides(pkgFile.provides);
  reader.fillConflicts(pkgFile.conflicts);
  reader.fillObsoletes(pkgFile.obsoletes);
  reader.fillRequires(pkgFile.requires);
  reader.fillChangeLog(pkgFile.changeLog);
  reader.fillFileList(pkgFile.fileList);
  reader.close();
  pkgFile.fileName = fileName;
}

bool RpmBackEnd::validPkgFileName(const std::string& fileName) const
{
  if (fileName.length() <= 4)
    return 0;
  const std::string ext = ".rpm";
  for(std::string::size_type i = 0;i < ext.size();i++)
    if (fileName[fileName.length() - ext.length() + i] != ext[i])
      return 0;
  return 1;
}

bool RpmBackEnd::validSourcePkgFileName(const std::string& fileName) const
{
  if (fileName.length() <= 8)
    return 0;
  const std::string ext = ".src.rpm";
  for(std::string::size_type i = 0;i < ext.size();i++)
    if (fileName[fileName.length() - ext.length() + i] != ext[i])
      return 0;
  return 1;
}

int buildSenseFlags(const VersionCond& c)
{
  int value = 0;
  if (c.isEqual())
    value |= RPMSENSE_EQUAL;
  if (c.isLess())
    value |= RPMSENSE_LESS;
  if (c.isGreater())
    value |= RPMSENSE_GREATER;
  return value;
}

std::auto_ptr<AbstractPackageBackEnd> createRpmBackEnd()
{
  return std::auto_ptr<AbstractPackageBackEnd>(new RpmBackEnd());
}

bool RpmBackEnd::transaction(const StringVector& toInstall,
			     const StringVector& toRemove,
			     const StringToStringMap& toUpgrade,
			     const StringToStringMap& toDowngrade)
{
  if (!toDowngrade.empty())
    throw NotImplementedException("Downgrading tasks in a transaction");
  logMsg(LOG_DEBUG, "backend:starting transaction with %zu packages to install, %zu packages to remove, %zu packages to upgrade and %zu packages to downgrade", toInstall.size(), toRemove.size(), toUpgrade.size(), toDowngrade.size());
  for(StringVector::size_type i = 0;i < toInstall.size();i++)
    logMsg(LOG_DEBUG, "backend:%s to install", toInstall[i].c_str());
  for(StringVector::size_type i = 0;i < toRemove.size();i++)
    logMsg(LOG_DEBUG, "backend:%s to remove", toRemove[i].c_str());
  for(StringToStringMap::const_iterator it = toUpgrade.begin();it != toUpgrade.end();it++)
    logMsg(LOG_DEBUG, "backend:%s -> %s to upgrade", it->first.c_str(), it->second.c_str());
  for(StringToStringMap::const_iterator it = toDowngrade.begin();it != toDowngrade.end();it++)
    logMsg(LOG_DEBUG, "backend:%s -> %s to downgrade", it->first.c_str(), it->second.c_str());
  RpmTransaction transact;
  transact.init();
  transact.process(toInstall, toRemove, toUpgrade, toDowngrade);
  transact.close();
  logMsg(LOG_DEBUG, "backend:transaction is completed");
  return 1;
}

DEEPSOLVER_END_NAMESPACE
