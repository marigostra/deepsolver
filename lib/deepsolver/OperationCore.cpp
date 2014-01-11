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
#include"deepsolver/OperationCore.h"
#include"deepsolver/Repository.h"
#include"deepsolver/PkgInfoProcessor.h"
#include"deepsolver/FilesFetch.h"
#include"deepsolver/AbstractPkgBackEnd.h"
#include"deepsolver/AbstractTaskSolver.h"
#include"deepsolver/PkgScope.h"
#include"deepsolver/PkgSnapshot.h"

DEEPSOLVER_BEGIN_NAMESPACE

namespace
{
  void fillWithhInstalledPackages(AbstractPkgBackEnd& backend,
				  PkgSnapshot::Snapshot& snapshot,
				  ConstCharVector& strings,
				  bool stopOnInvalidPkg)
  {
    PkgSnapshot::removeEqualPkgs(snapshot);
    PkgSnapshot::PkgVector& pkgs = snapshot.pkgs;
    AbstractInstalledPkgIterator::Ptr it = backend.enumInstalledPkg();
    size_t installedCount = 0;
    PkgVector toInhanceWith;
    Pkg pkg;
    while(it->moveNext(pkg))
      {
	if (!pkg.valid())
	  {
	    if (stopOnInvalidPkg)
	      throw OperationCoreException(OperationCoreException::InvalidInstalledPkg); else //FIXME:Add package designation here;
	      logMsg(LOG_WARNING, "OS has an invalid package: %s", backend.getDesignation(pkg, AbstractPkgBackEnd::EpochIfNonZero).c_str());
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
    logMsg(LOG_DEBUG, "operation:the system has %zu installed packages, %zu of them should be added to the existing snapshot", installedCount, toInhanceWith.size());
    PkgSnapshot::enhance(snapshot, toInhanceWith, PkgFlagInstalled, strings);
  }

  void fillUpgradeDowngrade(const AbstractPkgBackEnd& backend,
			    const AbstractPkgScope& scope,
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
	  if (remove[j] != BadVarId && scope.pkgIdOfVarId(remove[j]) == scope.pkgIdOfVarId(install[i]))
	    break;
	if (j >= remove.size())
	  continue;
	const std::string versionToInstall = scope.getVersionDef(install[i]);
	const std::string versionToRemove = scope.getVersionDef(remove[j]);
	if (!backend.verEqual(versionToInstall, versionToRemove))//Packages may be  with equal versions but with different build times;
	  {
	    if (backend.verGreater(versionToInstall, versionToRemove))
	      upgrade.insert(VarIdToVarIdMap::value_type(remove[j], install[i])); else
	      downgrade.insert(VarIdToVarIdMap::value_type(remove[j], install[i]));
	    install[i] = BadVarId;
	  } else
	  logMsg(LOG_WARNING, "upgrade:trying to upgrade packages with same version but with different build time: %s and %s", scope.getDesignationDef(install[i]).c_str(), scope.getDesignationDef(remove[j]).c_str());
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
}

TransactionIterator::Ptr OperationCore::transaction(AbstractTransactionListener& listener, const UserTask& userTask)
{
  const ConfRoot& root = m_conf.root();
  freeAutoReleaseStrings();
  for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
    File::readAhead(root.os.transactReadAhead[i]);
  AbstractPkgBackEnd::Ptr backend = CREATE_PKG_BACKEND;
  backend->initialize();
  PkgSnapshot::Snapshot snapshot;
  listener.onPkgListProcessingBegin();
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  if (snapshot.pkgs.empty())//FIXME:
    throw NotImplementedException("Empty set of attached repositories");
  fillWithhInstalledPackages(*backend.get(), snapshot, m_autoReleaseStrings, root.stopOnInvalidInstalledPkg);
  PkgScope scope(*backend.get(), snapshot);
  scope.initMetadata();
  listener.onPkgListProcessingEnd();
  TaskSolverData taskSolverData(*backend.get(), scope, m_conf);
  AbstractTaskSolver::Ptr solver = createTaskSolver(taskSolverData);
  VarIdVector toInstall, toRemove;
  solver->solve(userTask, toInstall, toRemove);
  VarIdToVarIdMap toUpgrade, toDowngrade;
  fillUpgradeDowngrade(*backend.get(), scope, toInstall, toRemove, toUpgrade, toDowngrade);
  PkgVector pkgInstall, pkgRemove, pkgUpgradeFrom, pkgUpgradeTo, pkgDowngradeFrom, pkgDowngradeTo;
  for(VarIdVector::size_type i = 0;i < toInstall.size();i++)
    {
      Pkg pkg;
      scope.fullPkgData(toInstall[i], pkg);
      pkgInstall.push_back(pkg);
    }
  for(VarIdVector::size_type i = 0;i < toRemove.size();i++)
    {
      Pkg pkg;
      scope.fullPkgData(toRemove[i], pkg);
      pkgRemove.push_back(pkg);
    }
  for(VarIdToVarIdMap::const_iterator it = toUpgrade.begin();it != toUpgrade.end();it++)
    {
      Pkg pkg1, pkg2;
      scope.fullPkgData(it->first, pkg1);
      scope.fullPkgData(it->second, pkg2);
      pkgUpgradeFrom.push_back(pkg1);
      pkgUpgradeTo.push_back(pkg2);
    }
  for(VarIdToVarIdMap::const_iterator it = toDowngrade.begin();it != toDowngrade.end();it++)
    {
      Pkg pkg1, pkg2;
      scope.fullPkgData(it->first, pkg1);
      scope.fullPkgData(it->second, pkg2);
      pkgDowngradeFrom.push_back(pkg1);
      pkgDowngradeTo.push_back(pkg2);
    }
  freeAutoReleaseStrings();
  return TransactionIterator::Ptr(new TransactionIterator(m_conf, backend,
								    pkgInstall, pkgRemove,
								    pkgUpgradeFrom, pkgUpgradeTo,
								    pkgDowngradeFrom, pkgDowngradeTo));
}

void OperationCore::closure(const UserTaskItemToInstallVector& toInstall, PkgVector& res)
{
  const ConfRoot& root = m_conf.root();
  res.clear();
  freeAutoReleaseStrings();
  AbstractPkgBackEnd::Ptr backend = CREATE_PKG_BACKEND;
  backend->initialize();
  PkgSnapshot::Snapshot snapshot;
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(root.dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  if (snapshot.pkgs.empty() && toInstall.empty())
    return;
  UserTask task;
  task.itemsToInstall = toInstall;
  PkgScope scope(*backend.get(), snapshot);
  scope.initMetadata();
  TaskSolverData taskSolverData(*backend.get(), scope, m_conf);
  AbstractTaskSolver::Ptr solver = createTaskSolver(taskSolverData);
  VarIdVector install, remove;
  solver->solve(task, install, remove);
  assert(remove.empty());
  for(VarIdVector::size_type i = 0;i < install.size();i++)
    {
      Pkg pkg;
      scope.fullPkgData(install[i], pkg);
      res.push_back(pkg);
    }
  freeAutoReleaseStrings();
}

void OperationCore::fetchMetadata(AbstractFetchListener& listener,
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
  if (Directory::exists(tmpDir))
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
  PkgInfoProcessor infoProcessor;
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

void OperationCore::generateSat(AbstractTransactionListener& listener,
				const UserTask& userTask,
				std::ostream& s)
{
  const ConfRoot& root = m_conf.root();
  freeAutoReleaseStrings();
  for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
    File::readAhead(root.os.transactReadAhead[i]);
  AbstractPkgBackEnd::Ptr backend = CREATE_PKG_BACKEND;
  backend->initialize();
  PkgSnapshot::Snapshot snapshot;
  listener.onPkgListProcessingBegin();
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  if (snapshot.pkgs.empty())//FIXME:
    throw NotImplementedException("Empty set of attached repositories");
  fillWithhInstalledPackages(*backend.get(), snapshot, m_autoReleaseStrings, root.stopOnInvalidInstalledPkg);
  PkgScope scope(*backend.get(), snapshot);
  scope.initMetadata();
  listener.onPkgListProcessingEnd();
  TaskSolverData taskSolverData(*backend.get(), scope, m_conf);
  AbstractTaskSolver::Ptr solver = createTaskSolver(taskSolverData);
  solver->dumpSat(userTask, s);
}

void OperationCore::printPackagesByRequire(const NamedPkgRel& rel, std::ostream& s)
{
  const ConfRoot& root = m_conf.root();
  freeAutoReleaseStrings();
  for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
    File::readAhead(root.os.transactReadAhead[i]);
  AbstractPkgBackEnd::Ptr backend = CREATE_PKG_BACKEND;
  backend->initialize();
  PkgSnapshot::Snapshot snapshot;
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  fillWithhInstalledPackages(*backend.get(), snapshot, m_autoReleaseStrings, root.stopOnInvalidInstalledPkg);
  PkgScope scope(*backend.get(), snapshot);
  scope.initMetadata();
  if (!scope.knownPkgName(rel.pkgName))
    {
      logMsg(LOG_DEBUG, "operation:package name \'%s\' is unknown", rel.pkgName.c_str());
      return;
    }
  const PackageId pkgId = scope.strToPkgId(rel.pkgName);
  const IdPkgRel idPkgRel(pkgId, rel.type, rel.ver);
  VarIdVector vars;
  scope.selectMatchingVarsWithProvides(idPkgRel, vars);
  noDoubling(vars);
  for(VarIdVector::size_type i = 0;i < vars.size();++i)
    {
      s << scope.getDesignationDef(vars[i]);
      if (scope.isInstalled(vars[i]))
	s << " (installed)";
      s << std::endl;
      }
  freeAutoReleaseStrings();
}

void OperationCore::printSnapshot(bool withInstalled, 
				  bool withIds,
				  std::ostream& s)
{
  logMsg(LOG_DEBUG, withInstalled?"operation:printing snapshot with installed packages":"operation:printing snapshot without installed packages"); 
  const ConfRoot& root = m_conf.root();
  freeAutoReleaseStrings();
  if (withInstalled)
    {
      for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
	File::readAhead(root.os.transactReadAhead[i]);
    }
  AbstractPkgBackEnd::Ptr backEnd = CREATE_PKG_BACKEND;
  backEnd->initialize();
  PkgSnapshot::Snapshot snapshot;
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  if (withInstalled)
    fillWithhInstalledPackages(*backEnd.get(), snapshot, m_autoReleaseStrings, root.stopOnInvalidInstalledPkg);
  PkgSnapshot::printContent(snapshot, withIds, s);
  freeAutoReleaseStrings();
}

void OperationCore::getPkgNames(bool withInstalled, StringVector& res)
{
  const ConfRoot& root = m_conf.root();
  freeAutoReleaseStrings();
  for(StringVector::size_type i = 0;i < root.os.transactReadAhead.size();i++)
    File::readAhead(root.os.transactReadAhead[i]);
  AbstractPkgBackEnd::Ptr backend = CREATE_PKG_BACKEND;
  backend->initialize();
  PkgSnapshot::Snapshot snapshot;
  PkgSnapshot::loadFromFile(snapshot, Directory::mixNameComponents(root.dir.pkgData, PKG_DATA_FILE_NAME), m_autoReleaseStrings);
  if (withInstalled)
    fillWithhInstalledPackages(*backend.get(), snapshot, m_autoReleaseStrings, root.stopOnInvalidInstalledPkg);
  StringSet names;
  for(PkgSnapshot::PkgVector::size_type i = 0;i < snapshot.pkgs.size();++i)
    {
      assert(snapshot.pkgs[i].pkgId < snapshot.pkgNames.size());
      names.insert(snapshot.pkgNames[snapshot.pkgs[i].pkgId]);
    }
  res.clear();
  for(StringSet::const_iterator it = names.begin();it != names.end();++it)
    res.push_back(*it);
  freeAutoReleaseStrings();
}

DEEPSOLVER_END_NAMESPACE
