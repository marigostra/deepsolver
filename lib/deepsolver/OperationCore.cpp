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
#include"deepsolver/OperationCore.h"
#include"deepsolver/Repository.h"
#include"deepsolver/PackageInfoProcessor.h"
#include"deepsolver/FilesFetch.h"
#include"deepsolver/AbstractPackageBackEnd.h"
#include"deepsolver/AbstractTaskSolver.h"
#include"deepsolver/AbstractSatSolver.h"
#include"deepsolver/PkgScope.h"
#include"deepsolver/SatWriter.h"
#include"deepsolver/PkgSnapshot.h"
#include"deepsolver/PkgUtils.h"

DEEPSOLVER_BEGIN_NAMESPACE

static std::string urlToFileName(const std::string& url);
static void buildTemporaryIndexFileNames(StringToStringMap& files, const std::string& tmpDirName);

void OperationCore::fetchIndices(AbstractFetchListener& listener,
				 const AbstractOperationContinueRequest& continueRequest)
{
  const ConfRoot& root = m_conf.root();
  const std::string tmpDir = Directory::mixNameComponents(root.dir.pkgData, PKG_DATA_FETCH_DIR);
  logMsg(LOG_DEBUG, "operation:package data updating begin: pkgdatadir=\'%s\', tmpdir=\'%s\'", root.dir.pkgData.c_str(), tmpDir.c_str());
  freeAutoReleaseStrings();
  listener.onHeadersFetch();
  //FIXME:file lock;
  RepositoryVector repo;
  for(ConfRepoVector::size_type i = 0;i < root.repo.size();i++)
    {
      if (!root.repo[i].enabled)
	continue;
      if (root.repo[i].takeSources)//FIXME:
	throw NotImplementedException("Source packages support");
      if (root.repo[i].takeDescr)//FIXME:
	throw NotImplementedException("Package descriptions support");
      if (root.repo[i].takeFileList)//FIXME:
	throw NotImplementedException("Package file lists support");
      for(StringVector::size_type k = 0;k < root.repo[i].arch.size();k++)
	for(StringVector::size_type j = 0;j < root.repo[i].components.size();j++)
	  {
	    const std::string& arch = root.repo[i].arch[k];
	    const std::string& component = root.repo[i].components[j];
	    logMsg(LOG_DEBUG, "operation:registering repo \'%s\' for index update (\'%s\', %s, %s)", root.repo[i].name.c_str(), root.repo[i].url.c_str(), arch.c_str(), component.c_str());
	    repo.push_back(Repository(root.stopOnInvalidRepoPkg, root.tinyFileSizeLimit, root.repo[i], arch, component));
	  }
    }
  StringToStringMap files;
  for(RepositoryVector::size_type i = 0;i < repo.size();i++)
    {
      repo[i].fetchInfoAndChecksum();
      repo[i].addIndexFilesForFetch(files);
    }
  buildTemporaryIndexFileNames(files, tmpDir);
  logMsg(LOG_DEBUG, "operation:list of index files to download consists of %zu entries:", files.size());
  for(StringToStringMap::const_iterator it = files.begin();it != files.end();it++)
    logMsg(LOG_DEBUG, "operation:download entry: \'%s\' -> \'%s\'", it->first.c_str(), it->second.c_str());
  if (Directory::isExist(tmpDir))
    logMsg(LOG_WARNING, "operation:directory \'%s\' already exists, probably unfinished previous transaction", tmpDir.c_str());
  StringToStringMap remoteFiles;
  for(StringToStringMap::iterator it = files.begin();it != files.end();it++)
    {
      std::string localFileName;
      if (!FilesFetch::isLocalFileUrl(it->first, localFileName))
	remoteFiles.insert(StringToStringMap::value_type(it->first, it->second)); else
	it->second = localFileName;
    }
  Directory::ensureExistsAndEmpty(tmpDir, 1);//1 means erase any content;
  if (!remoteFiles.empty())
    {
      logMsg(LOG_DEBUG, "operation:need to fetch %zu remote files", remoteFiles.size());
  listener.onFetchBegin();
      FilesFetch fetch(listener, continueRequest);
      fetch.fetch(remoteFiles);
      listener.onFetchIsCompleted();
    }
  listener.onFilesReading();
  PkgSnapshot::Snapshot snapshot;
  StringToPkgIdMap stringToPkgIdMap;
  PkgSnapshot::PkgRecipientAdapter snapshotAdapter(snapshot, m_autoReleaseStrings, stringToPkgIdMap);
  PkgUrlsFile urlsFile(m_conf);
  PackageInfoProcessor infoProcessor;
  urlsFile.open();
  for(RepositoryVector::size_type i = 0; i < repo.size();i++)
    repo[i].loadPackageData(files, snapshotAdapter, urlsFile, infoProcessor);
  PkgSnapshot::rearrangeNames(snapshot);
  std::sort(snapshot.pkgs.begin(), snapshot.pkgs.end());
  const std::string outputFileName = Directory::mixNameComponents(root.dir.pkgData, PKG_DATA_FILE_NAME);
  logMsg(LOG_DEBUG, "operation:saving constructed data to \'%s\', score is %zu", outputFileName.c_str(), PkgSnapshot::getScore(snapshot));
  PkgSnapshot::saveToFile(snapshot, outputFileName, m_autoReleaseStrings);
  urlsFile.close();
  freeAutoReleaseStrings();
  //FIXME:The current code is working but it should create temporary file elsewhere and then replace with it already existing outputFileName;
  logMsg(LOG_DEBUG, "operation:clearing and removing \'%s\'", tmpDir.c_str());
  Directory::eraseContent(tmpDir);
  Directory::remove(tmpDir);
  logMsg(LOG_INFO, "Repository index updating finished!");
  //FIXME:remove file lock;
}

std::auto_ptr<TransactionIterator> OperationCore::transaction(AbstractTransactionListener& listener, const UserTask& userTask)
{
  const ConfRoot& root = m_conf.root();
  freeAutoReleaseStrings();
  for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
    File::readAhead(root.os.transactReadAhead[i]);
  std::auto_ptr<AbstractPackageBackEnd> backEnd = CREATE_PACKAGE_BACKEND;
  backEnd->initialize();
  PkgSnapshot::Snapshot snapshot;
  ProvideMap provideMap;
  InstalledReferences requiresReferences, conflictsReferences;
  listener.onPkgListProcessingBegin();
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  logMsg(LOG_DEBUG, "operation:the score of the snapshot just loaded is %zu", PkgSnapshot::getScore(snapshot));
  if (snapshot.pkgs.empty())//FIXME:
    throw NotImplementedException("Empty set of attached repositories");
  PkgUtils::fillWithhInstalledPackages(*backEnd.get(), snapshot, m_autoReleaseStrings, root.stopOnInvalidInstalledPkg);
  PkgUtils::prepareReversedMaps(snapshot, provideMap, requiresReferences, conflictsReferences);
  listener.onPkgListProcessingEnd();
  PkgScope scope(*backEnd.get(), snapshot, provideMap, requiresReferences, conflictsReferences);
  TaskSolverData taskSolverData(*backEnd.get(), scope);
  for(ConfProvideVector::size_type i = 0;i < root.provide.size();i++)
    {
      assert(!trim(root.provide[i].name).empty());
      TaskSolverProvideInfo info(root.provide[i].name);
      for(StringVector::size_type k = 0;k < root.provide[i].providers.size();k++)
	info.providers.push_back(root.provide[i].providers[k]);
      taskSolverData.provides.push_back(info);
    }
  std::auto_ptr<AbstractTaskSolver> solver = createTaskSolver(taskSolverData);
  VarIdVector toInstall, toRemove;
  solver->solve(userTask, toInstall, toRemove);
  VarIdToVarIdMap toUpgrade, toDowngrade;
  PkgUtils::fillUpgradeDowngrade(*backEnd.get(), scope, toInstall, toRemove, toUpgrade, toDowngrade);
  PkgVector pkgInstall, pkgRemove, pkgUpgradeFrom, pkgUpgradeTo, pkgDowngradeFrom, pkgDowngradeTo;
  for(VarIdVector::size_type i = 0;i < toInstall.size();i++)
    {
      Pkg pkg;
      scope.fillPkgData(toInstall[i], pkg);
      pkgInstall.push_back(pkg);
    }
  for(VarIdVector::size_type i = 0;i < toRemove.size();i++)
    {
      Pkg pkg;
      scope.fillPkgData(toRemove[i], pkg);
      pkgRemove.push_back(pkg);
    }
  for(VarIdToVarIdMap::const_iterator it = toUpgrade.begin();it != toUpgrade.end();it++)
    {
      Pkg pkg1, pkg2;
      scope.fillPkgData(it->first, pkg1);
      scope.fillPkgData(it->second, pkg2);
      pkgUpgradeFrom.push_back(pkg1);
      pkgUpgradeTo.push_back(pkg2);
    }
  for(VarIdToVarIdMap::const_iterator it = toDowngrade.begin();it != toDowngrade.end();it++)
    {
      Pkg pkg1, pkg2;
      scope.fillPkgData(it->first, pkg1);
      scope.fillPkgData(it->second, pkg2);
      pkgDowngradeFrom.push_back(pkg1);
      pkgDowngradeTo.push_back(pkg2);
    }
  return std::auto_ptr<TransactionIterator>(new TransactionIterator(m_conf, backEnd,
								    pkgInstall, pkgRemove,
								    pkgUpgradeFrom, pkgUpgradeTo,
								    pkgDowngradeFrom, pkgDowngradeTo));
  freeAutoReleaseStrings();
}

std::string OperationCore::generateSat(AbstractTransactionListener& listener, const UserTask& userTask)
{
  const ConfRoot& root = m_conf.root();
  freeAutoReleaseStrings();
  for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
    File::readAhead(root.os.transactReadAhead[i]);
  std::auto_ptr<AbstractPackageBackEnd> backEnd = CREATE_PACKAGE_BACKEND;
  backEnd->initialize();
  PkgSnapshot::Snapshot snapshot;
  ProvideMap provideMap;
  InstalledReferences requiresReferences, conflictsReferences;
  listener.onPkgListProcessingBegin();
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  logMsg(LOG_DEBUG, "operation:the score of the snapshot just loaded is %zu", PkgSnapshot::getScore(snapshot));
  if (snapshot.pkgs.empty())//FIXME:
    throw NotImplementedException("Empty set of attached repositories");
  PkgUtils::fillWithhInstalledPackages(*backEnd.get(), snapshot, m_autoReleaseStrings, root.stopOnInvalidInstalledPkg);
  PkgUtils::prepareReversedMaps(snapshot, provideMap, requiresReferences, conflictsReferences);
  listener.onPkgListProcessingEnd();
  PkgScope scope(*backEnd.get(), snapshot, provideMap, requiresReferences, conflictsReferences);
  TaskSolverData taskSolverData(*backEnd.get(), scope);
  for(ConfProvideVector::size_type i = 0;i < root.provide.size();i++)
    {
      assert(!trim(root.provide[i].name).empty());
      TaskSolverProvideInfo info(root.provide[i].name);
      for(StringVector::size_type k = 0;k < root.provide[i].providers.size();k++)
	info.providers.push_back(root.provide[i].providers[k]);
      taskSolverData.provides.push_back(info);
    }
  SatWriter writer(taskSolverData);
  const std::string res = writer.generateSat(userTask);
  freeAutoReleaseStrings();
  return res;
}

void OperationCore::printPackagesByRequire(const NamedPkgRel& rel, std::ostream& s)
{
  const ConfRoot& root = m_conf.root();
  freeAutoReleaseStrings();
  for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
    File::readAhead(root.os.transactReadAhead[i]);
  std::auto_ptr<AbstractPackageBackEnd> backEnd = CREATE_PACKAGE_BACKEND;
  backEnd->initialize();
  PkgSnapshot::Snapshot snapshot;
  ProvideMap provideMap;
  InstalledReferences requiresReferences, conflictsReferences;
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  logMsg(LOG_DEBUG, "operation:the score of the snapshot just loaded is %zu", PkgSnapshot::getScore(snapshot));
  if (snapshot.pkgs.empty())//FIXME:
    throw NotImplementedException("Empty set of attached repositories");
  PkgUtils::fillWithhInstalledPackages(*backEnd.get(), snapshot, m_autoReleaseStrings, root.stopOnInvalidInstalledPkg);
  PkgUtils::prepareReversedMaps(snapshot, provideMap, requiresReferences, conflictsReferences);
  PkgScope scope(*backEnd.get(), snapshot, provideMap, requiresReferences, conflictsReferences);
  if (!scope.checkName(rel.pkgName))
    {
      logMsg(LOG_DEBUG, "operation:package name \'%s\' is unknown", rel.pkgName.c_str());
      return;
    }
  const PackageId pkgId = scope.strToPackageId(rel.pkgName);
  const IdPkgRel idPkgRel(pkgId, rel.type, rel.ver);
  logMsg(LOG_DEBUG, "operation:processing %zu %s", idPkgRel.pkgId, idPkgRel.verString().c_str());
  VarIdVector vars;
  scope.selectMatchingVarsWithProvides(idPkgRel, vars);
  rmDub(vars);
  logMsg(LOG_DEBUG, "operation:found %zu packages matching given require", vars.size());
  for(VarIdVector::size_type i = 0;i < vars.size();i++)
    {
      s << scope.constructPackageName(vars[i]);
      if (scope.isInstalled(vars[i]))
	s << " (installed)";
      s << std::endl;
      }
  freeAutoReleaseStrings();
}

//Static functions;

std::string urlToFileName(const std::string& url)
{
  std::string s;
  for(std::string::size_type i = 0;i < url.length();i++)
    {
      const char c = url[i];
      if ((c >= 'a' && c <= 'z') ||
	  (c >= 'A' && c <= 'Z') ||
	  (c >= '0' && c<= '9') ||
	  c == '-' ||
	  c == '_')
	s += c; else 
	if (s.empty() || s[s.length() - 1] != '-')
	  s += '-';
    } //for();
  return s;
}

void buildTemporaryIndexFileNames(StringToStringMap& files, const std::string& tmpDirName)
{
  for(StringToStringMap::iterator it = files.begin();it != files.end();it++)
    it->second = Directory::mixNameComponents(tmpDirName, urlToFileName(it->first));
}

DEEPSOLVER_END_NAMESPACE
