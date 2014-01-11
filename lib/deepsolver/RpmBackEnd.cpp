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
#include"deepsolver/RpmBackEnd.h"
#include"deepsolver/RpmFileHeaderReader.h"
#include"deepsolver/RpmTransaction.h"

DEEPSOLVER_BEGIN_NAMESPACE

static bool alreadyReadConfigFiles = 0;
static int buildSenseFlags(const VerSubset& c);

void RpmBackEnd::initialize()
{
  if (!alreadyReadConfigFiles)
    {
      rpmReadConfigFiles( NULL, NULL );                                                                                                                          
      alreadyReadConfigFiles = 1;
    }
}

int RpmBackEnd::verCmp(const std::string& ver1, const std::string& ver2) const
{
  return rpmvercmp(ver1.c_str(), ver2.c_str());
}

bool RpmBackEnd::verOverlap(const VerSubset& ver1, const VerSubset& ver2) const
{
  return rpmRangesOverlap("", ver1.version.c_str(), buildSenseFlags(ver1),
			  "", ver2.version.c_str(), buildSenseFlags(ver2));
}

bool RpmBackEnd::verEqual(const std::string& ver1, const std::string& ver2) const
{
  return verOverlap(VerSubset(ver1), VerSubset(ver2));
}

bool RpmBackEnd::verGreater(const std::string& ver1, const std::string& ver2) const
{
  return verOverlap(VerSubset(ver1, VerLess), VerSubset(ver2));
}

AbstractInstalledPkgIterator::Ptr RpmBackEnd::enumInstalledPkg() const
{
  RpmInstalledPkgIterator::Ptr rpmIt(new RpmInstalledPkgIterator());
  rpmIt->openEnum();
  return rpmIt;
}

void RpmBackEnd::readPkgFile(const std::string& fileName, PkgFile& pkgFile) const
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

int buildSenseFlags(const VerSubset& c)
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

AbstractPkgBackEnd::Ptr createRpmBackEnd()
{
  return AbstractPkgBackEnd::Ptr(new RpmBackEnd());
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

std::string RpmBackEnd::makeVer(int epoch,
				const std::string& ver,
				const std::string& release,
				int epochMode) const
{
  std::ostringstream res;
  switch(epochMode)
    {
    case EpochAlways:
      res << epoch << ":";
      break;
    case EpochIfNonZero:
      if (epoch > 0)
  res << epoch << ":";
  break;
    };
  res << (!ver.empty()?ver:"NO_VERSION") << "-" << (!release.empty()?release:"NO_RELEASE");
  return res.str();
}

std::string RpmBackEnd::makeVer(const PkgBase& pkg, int epochMode) const
{
  return makeVer(pkg.epoch, pkg.version, pkg.release, epochMode);
}

std::string RpmBackEnd::combineNameAndVer(const std::string& name, const std::string& ver) const
{
  assert(!name.empty() && !ver.empty());
  return name + "-" + ver;
}

std::string RpmBackEnd::getDesignation(const PkgBase& pkg, int epochMode) const
{
  return combineNameAndVer(pkg.name, makeVer(pkg, epochMode));
}

std::string RpmBackEnd::getDesignation(const NamedPkgRel& rel) const
{
  if (!rel.verRestricted())
    return rel.pkgName;
  std::string res = rel.pkgName;
  res += " ";
  if (rel.type & VerLess)
    res += "<";
  if (rel.type & VerGreater)
    res += ">";
  if (rel.type & VerEquals)
    res += "=";
  res += " " + rel.ver;
  return res;
}

bool RpmBackEnd::matches(const NamedPkgRel& rel, const Pkg& pkg) const
{
  assert(!rel.pkgName.empty() && !pkg.name.empty());
  assert(!pkg.version.empty() && !pkg.release.empty());
  if (rel.pkgName == pkg.name)
    {
      if (!rel.verRestricted())
	return 1;
      if (verOverlap(VerSubset(makeVer(pkg, EpochAlways)),
		     VerSubset(rel.ver, rel.type)))
	return 1;
    }
  for (NamedPkgRelVector::size_type i = 0;i < pkg.provides.size();++i)
    if (matches(rel, pkg.provides[i]))
      return 1;
  return 0;
}

bool RpmBackEnd::matches(const NamedPkgRel& rel, const NamedPkgRel& provide) const
{
  if (rel.pkgName != provide.pkgName)
    return 0;
  if (!rel.verRestricted())
    return 1;
  if (rel.verRestricted() && provide.verRestricted() &&
      verOverlap(VerSubset(provide.ver, provide.type),
		 VerSubset(rel.ver, rel.type)))
    return 1;
  return 0;
}

bool RpmBackEnd::matches(const IdPkgRel& rel, const IdPkgRel& provide) const
{
  if (rel.pkgId != provide.pkgId)
    return 0;
  if (!rel.verRestricted() && !provide.verRestricted())
    return 1;
  if (rel.verRestricted() && provide.verRestricted() &&
      verOverlap(VerSubset(provide.ver, provide.verDir),
		 VerSubset(rel.ver, rel.verDir)))
    return 1;
  return 0;
}

bool RpmBackEnd::theSamePkg(const PkgBase& pkg1, const PkgBase& pkg2) const
{
  return (pkg1.name == pkg2.name &&
	  pkg1.epoch == pkg2.epoch &&
	  pkg1.version == pkg2.version &&
	  pkg1.release == pkg2.release &&
	  pkg1.buildTime == pkg2.buildTime);
}

DEEPSOLVER_END_NAMESPACE
