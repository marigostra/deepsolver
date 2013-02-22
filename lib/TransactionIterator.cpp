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

#include"deepsolver.h"
#include"TransactionIterator.h"
#include"PkgUrlsFile.h"
#include"FilesFetch.h"

void TransactionIterator::fetchPackages(AbstractFetchListener& listener,
					const AbstractOperationContinueRequest& continueRequest)
{
  const ConfRoot& root = m_conf.root();
  const std::string dir = root.dir.pkgCache;
  assert(!dir.empty());
  logMsg(LOG_DEBUG, "transaction:need to fetch %zu packages to \'%s\'", m_install.size() + m_upgradeTo.size() + m_downgradeTo.size(), dir.c_str());
  listener.onFetchBegin();
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
      throw OperationException(OperationException::InternalIOProblem);
	}
      installFileNames[i] = File::baseNameFromUrl(installUrls[i]);
      if (trim(installFileNames[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:URL \'%s\' is invalid because it has no file name", installUrls[i].c_str());
      throw OperationException(OperationException::InternalIOProblem);
	}
    }
  for(StringVector::size_type i = 0;i < upgradeUrls.size();i++)
    {
      if (trim(upgradeUrls[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:package \'%s\' to upgrade to has no proper URL to fetch from", m_upgradeTo[i].name.c_str());
      throw OperationException(OperationException::InternalIOProblem);
	}
      upgradeFileNames[i] = File::baseNameFromUrl(upgradeUrls[i]);
      if (trim(upgradeFileNames[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:URL \'%s\' is invalid because it has no file name", upgradeUrls[i].c_str());
      throw OperationException(OperationException::InternalIOProblem);
	}
    }
  for(StringVector::size_type i = 0;i < downgradeUrls.size();i++)
    {
      if (trim(downgradeUrls[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:package \'%s\' to downgrade to has no proper URL to fetch from", m_downgradeTo[i].name.c_str());
      throw OperationException(OperationException::InternalIOProblem);
	}
      downgradeFileNames[i] = File::baseNameFromUrl(downgradeUrls[i]);
      if (trim(downgradeFileNames[i]).empty())
	{
	  logMsg(LOG_ERR, "transaction:URL \'%s\' is invalid because it has no file name", downgradeUrls[i].c_str());
      throw OperationException(OperationException::InternalIOProblem);
	}
    }
  logMsg(LOG_DEBUG, "transaction:preparing directory \'%s\'", dir.c_str());
  Directory::ensureExists(dir);
  FilesFetch fetch(listener, continueRequest);
  StringToStringMap fetchMap;
  assert(installUrls.size() == installFileNames.size());
  for(StringVector::size_type i = 0;i < installUrls.size();i++)
    fetchMap.insert(StringToStringMap::value_type(installUrls[i], Directory::mixNameComponents(dir, installFileNames[i])));
  for(StringVector::size_type i = 0;i < upgradeUrls.size();i++)
    fetchMap.insert(StringToStringMap::value_type(upgradeUrls[i], Directory::mixNameComponents(dir, upgradeFileNames[i])));
  for(StringVector::size_type i = 0;i < downgradeUrls.size();i++)
    fetchMap.insert(StringToStringMap::value_type(downgradeUrls[i], Directory::mixNameComponents(dir, downgradeFileNames[i])));
  logMsg(LOG_DEBUG, "transaction:starting fetching, fetch map contains %zu items", fetchMap.size());
  fetch.fetch(fetchMap);
  listener.onFetchIsCompleted();
}
