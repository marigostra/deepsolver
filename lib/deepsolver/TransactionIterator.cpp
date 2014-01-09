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
#include"deepsolver/TransactionIterator.h"
#include"deepsolver/PkgUrlsFile.h"
#include"deepsolver/FilesFetch.h"

DEEPSOLVER_BEGIN_NAMESPACE

void TransactionIterator::getUrls(StringVector& toInstall,
				  StringVector& toUpgrade,
				  StringVector& toDowngrade) const
{
  PkgUrlsFile urlsFile(m_conf);
  urlsFile.readUrls(m_install, toInstall);
  urlsFile.readUrls(m_upgradeTo, toUpgrade);
  urlsFile.readUrls(m_downgradeTo, toDowngrade);
}

void TransactionIterator::fetchPackages(AbstractFetchListener& listener,
					const AbstractOperationContinueRequest& continueRequest)
{
  const ConfRoot& root = m_conf.root();
  const std::string dir = root.dir.pkgCache;
  assert(!dir.empty());
  logMsg(LOG_DEBUG, "transaction:need to fetch %zu packages to \'%s\'", m_install.size() + m_upgradeTo.size() + m_downgradeTo.size(), dir.c_str());
  StringVector installUrls, upgradeUrls, downgradeUrls;
  PkgUrlsFile urlsFile(m_conf);
  urlsFile.readUrls(m_install, installUrls);
  urlsFile.readUrls(m_upgradeTo, upgradeUrls);
  urlsFile.readUrls(m_downgradeTo, downgradeUrls);
  StringVector installFileNames, upgradeFileNames, downgradeFileNames;
  installFileNames.resize(installUrls.size());
  upgradeFileNames.resize(upgradeUrls.size());
  downgradeFileNames.resize(downgradeUrls.size());
  for(StringVector::size_type i = 0;i < installUrls.size();i++)
    {
      if (trim(installUrls[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:package \'%s\' to install has no proper URL to fetch from", m_install[i].name.c_str());
	  throw 0;//FIXME:InternalProblem;
	}
      installFileNames[i] = File::baseNameFromUrl(installUrls[i]);
      if (trim(installFileNames[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:URL \'%s\' is invalid because it has no file name", installUrls[i].c_str());
	  throw 0;//FIXME:InternalProblem;
	}
    }
  for(StringVector::size_type i = 0;i < upgradeUrls.size();i++)
    {
      if (trim(upgradeUrls[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:package \'%s\' to upgrade to has no proper URL to fetch from", m_upgradeTo[i].name.c_str());
	  throw 0;//FIXME:InternalProblem;
	}
      upgradeFileNames[i] = File::baseNameFromUrl(upgradeUrls[i]);
      if (trim(upgradeFileNames[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:URL \'%s\' is invalid because it has no file name", upgradeUrls[i].c_str());
	  throw 0;//FIXME:InternalProblem;
	}
    }
  for(StringVector::size_type i = 0;i < downgradeUrls.size();i++)
    {
      if (trim(downgradeUrls[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:package \'%s\' to downgrade to has no proper URL to fetch from", m_downgradeTo[i].name.c_str());
	  throw 0;//FIXME:InternalProblem;
	}
      downgradeFileNames[i] = File::baseNameFromUrl(downgradeUrls[i]);
      if (trim(downgradeFileNames[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:URL \'%s\' is invalid because it has no file name", downgradeUrls[i].c_str());
	  throw 0;//FIXME:InternalProblem;
	}
    }
  logMsg(LOG_DEBUG, "transaction:preparing directory \'%s\'", dir.c_str());
  Directory::ensureExistsAndEmpty(dir, 1);//1 means erase any content;
  FilesFetch fetch(listener, continueRequest);
  StringToStringMap fetchMap;
  assert(installUrls.size() == installFileNames.size());
  for(StringVector::size_type i = 0;i < installUrls.size();i++)
    if (!FilesFetch::isLocalFileUrl(installUrls[i]))
      fetchMap.insert(StringToStringMap::value_type(installUrls[i], Directory::mixNameComponents(dir, installFileNames[i])));
  assert(upgradeUrls.size() == upgradeFileNames.size());
  for(StringVector::size_type i = 0;i < upgradeUrls.size();i++)
    if (!FilesFetch::isLocalFileUrl(upgradeUrls[i]))
      fetchMap.insert(StringToStringMap::value_type(upgradeUrls[i], Directory::mixNameComponents(dir, upgradeFileNames[i])));
  assert(downgradeUrls.size() == downgradeFileNames.size());
  for(StringVector::size_type i = 0;i < downgradeUrls.size();i++)
    if (!FilesFetch::isLocalFileUrl(downgradeUrls[i]))
      fetchMap.insert(StringToStringMap::value_type(downgradeUrls[i], Directory::mixNameComponents(dir, downgradeFileNames[i])));
  if (!fetchMap.empty())
    {
      logMsg(LOG_DEBUG, "transaction:starting fetching, fetch map contains %zu items", fetchMap.size());
      listener.onFetchBegin();
      fetch.fetch(fetchMap);
      listener.onFetchIsCompleted();
    } else
    logMsg(LOG_DEBUG, "transaction:actually there is nothing to fetch");
  assert(m_install.size() == installFileNames.size());
  assert(m_upgradeTo.size() == upgradeFileNames.size());
  assert(m_downgradeTo.size() == downgradeFileNames.size());
  for(StringVector::size_type i = 0;i < installFileNames.size();i++)
    {
      std::string localFileName;
      if (!FilesFetch::isLocalFileUrl(installUrls[i], localFileName))
	m_filesInstall.push_back(Directory::mixNameComponents(dir, installFileNames[i])); else
	m_filesInstall.push_back(localFileName);
    }
  for(PkgVector::size_type i = 0;i < m_remove.size();i++)
    m_namesRemove.push_back(m_remove[i].name);
  for(StringVector::size_type i = 0;i < upgradeFileNames.size();i++)
    {
      std::string localFileName;
      if (!FilesFetch::isLocalFileUrl(upgradeUrls[i], localFileName))
	m_filesUpgrade.insert(StringToStringMap::value_type(m_upgradeTo[i].name, Directory::mixNameComponents(dir, upgradeFileNames[i]))); else
	m_filesUpgrade.insert(StringToStringMap::value_type(m_upgradeTo[i].name, localFileName));
    }
  for(StringVector::size_type i = 0;i < downgradeFileNames.size();i++)
    {
      std::string localFileName;
      if (!FilesFetch::isLocalFileUrl(downgradeFileNames[i], localFileName))
	m_filesDowngrade.insert(StringToStringMap::value_type(m_downgradeTo[i].name, Directory::mixNameComponents(dir, downgradeFileNames[i]))); else
	m_filesDowngrade.insert(StringToStringMap::value_type(m_downgradeTo[i].name, localFileName));
    }
}

void TransactionIterator::makeChanges()
{
  m_backend->transaction(m_filesInstall, m_namesRemove, m_filesUpgrade, m_filesDowngrade);
}

DEEPSOLVER_END_NAMESPACE
