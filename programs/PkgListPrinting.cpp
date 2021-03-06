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
#include"PkgListPrinting.h"

DEEPSOLVER_BEGIN_NAMESPACE

namespace 
{
  void printThreeColumns(const StringVector& items)
  {
    StringVector v = items;
    std::sort(v.begin(), v.end());
    if (v.empty())
      return;
    StringVector v1, v2, v3;
    for(StringVector::size_type i = 0;i < v.size();i += 3)
      {
	v1.push_back(v[i]);
	if (i + 1 < v.size())
	  v2.push_back(v[i + 1]);
	if (i + 2 < v.size())
	  v3.push_back(v[i + 2]);
      }
    std::string::size_type maxLen1 = 0, maxLen2 = 0;
    for(StringVector::size_type i = 0;i < v1.size();i++)
      if (v1[i].length() > maxLen1)
	maxLen1 = v1[i].length();
    for(StringVector::size_type i = 0;i < v2.size();i++)
      if (v2[i].length() > maxLen2)
	maxLen2 = v2[i].length();
    assert(maxLen1 > 0 && (v2.empty() || maxLen2 > 0));
    for(StringVector::size_type i = 0;i < v3.size();i++)
      {
	std::cout << v1[i];
	for(std::string::size_type k = v1[i].length();k < maxLen1 + 2;k++)
	  std::cout << " ";
	std::cout << v2[i];
	for(std::string::size_type k = v2[i].length();k < maxLen2 + 2;k++)
	  std::cout << " ";
	std::cout << v3[i] << std::endl;
      }
    if (v1.size() > v3.size())
      {
	std::cout << v1[v1.size() - 1];
	if (v2.size() > v3.size())
	  {
	    for(std::string::size_type k = v1[v1.size() - 1].length();k < maxLen1 + 2;k++)
	      std::cout << " ";
	    std::cout << v2[v2.size() - 1];
	  }
	std::cout << std::endl;
      }
  }
}

void PkgListPrinting::printSolution(const TransactionIterator& it, bool toLog) const
{
  StringVector install, remove, upgrade, downgrade;
  for(PkgVector::size_type i = 0;i < it.getInstall().size();++i)
    install.push_back(it.getInstall()[i].name);
  for(PkgVector::size_type i = 0;i < it.getRemove().size();++i)
    remove.push_back(it.getRemove()[i].name);
  for(PkgVector::size_type i = 0;i < it.getUpgradeTo().size();++i)
    upgrade.push_back(it.getUpgradeTo()[i].name);
  for(PkgVector::size_type i = 0;i < it.getDowngradeTo().size();++i)
    downgrade.push_back(it.getDowngradeTo()[i].name);
  if (toLog)
    {
      logMsg(LOG_INFO, "%zu to install", install.size());
      logMsg(LOG_INFO, "%zu to remove", remove.size());
      logMsg(LOG_INFO, "%zu to upgrade", upgrade.size());
      logMsg(LOG_INFO, "%zu to downgrade", downgrade.size());
      for(StringVector::size_type i = 0;i < install.size();++i)
	logMsg(LOG_INFO, "I %s", install[i].c_str());
      for(StringVector::size_type i = 0;i < remove.size();++i)
	logMsg(LOG_INFO, "R %s", remove[i].c_str());
      for(StringVector::size_type i = 0;i < upgrade.size();++i)
	logMsg(LOG_INFO, "U %s", upgrade[i].c_str());
      for(StringVector::size_type i = 0;i < downgrade.size();++i)
	logMsg(LOG_INFO, "D %s", downgrade[i].c_str());
      return;
    }
  columnsView(install, remove, upgrade, downgrade);
  std::cout << std::endl;
}

void PkgListPrinting::columnsView(const StringVector& install,
				      const StringVector& remove,
				      const StringVector& upgrade,
				      const StringVector& downgrade) const
{
  std::cout << std::endl;
  if (!install.empty())
    {
      std::cout << "The following package(s) must be installed:" << std::endl;
      printThreeColumns(install);
      std::cout << std::endl;
    }
  if (!remove.empty())
    {
      StringVector v;
      std::cout << "The following package(s) must be removed:" << std::endl;
      printThreeColumns(remove);
      std::cout << std::endl;
    }
  if (!upgrade.empty())
    {
      std::cout << "The following package(s) must be upgraded:" << std::endl;
      printThreeColumns(upgrade);
      std::cout << std::endl;
    }
  if (!downgrade.empty())
    {
      std::cout << "The following package(s) must be downgraded:" << std::endl;
      printThreeColumns(downgrade);
      std::cout << std::endl;
    }
  std::cout << install.size() << " package(s) to install, " <<
    remove.size() << " package(s) to remove, " <<
    upgrade.size() << " package(s) to upgrade, " <<
    downgrade.size() << " package(s) to downgrade" << std::endl;
}

DEEPSOLVER_END_NAMESPACE
