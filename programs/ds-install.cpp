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
#include"deepsolver/ExceptionMessagesEn.h"
#include"TransactionProgress.h"
#include"Messages.h"
#include"PackageListPrinting.h"
#include"AlwaysTrueContinueRequest.h"
#include"FilesFetchProgress.h"

using namespace Deepsolver;

class DsInstallCliParser: public CliParser
{
public:
  /**\brief The default constructor*/
  DsInstallCliParser() {}

  /**\brief The destructor*/
  virtual ~DsInstallCliParser() {}

protected:
  size_t recognizeCluster(const StringVector& params, int& mode) const
  {
  assert(!params.empty());
  if (mode == 0 && params[0] == "--")
    return 0;
  if (mode == 0 && findKey(params[0]) != (KeyVector::size_type)-1)
    return CliParser::recognizeCluster(params, mode);
  if (params.size() < 3 ||
      (params[1] != "=" && params[1] != "<=" && params[1] != ">=" && params[1] != "<" && params[1] != ">"))
    return CliParser::recognizeCluster(params, mode);
  return 2;
  }

  void parseCluster(const StringVector& cluster, int& mode)
  {
    if (mode == 0 && cluster.size() == 1 && cluster[0] == "--")
      {
	mode = 1;
	return ;
      }
    if (mode == 0 && findKey(cluster[0]) != (KeyVector::size_type)-1)
      {
	CliParser::parseCluster(cluster, mode);
	return;
      }
    if(cluster.size() != 1 && cluster.size() != 3)
      {
	CliParser::parseCluster(cluster, mode);
	return;
      }
    if (trim(cluster[0]).empty())
      return;
    if (cluster.size() == 1)
      {
	userTask.itemsToInstall.push_back(UserTaskItemToInstall(cluster[0]));
	return;
      }
    if (trim(cluster[2]).empty())
      return;
    VerDirection verDir = VerNone;
    if (cluster[1] == "=")
      verDir = VerEquals; else
      if (cluster[1] == "<=")
	verDir = VerLess | VerEquals; else
	if (cluster[1] == ">=")
	  verDir = VerGreater | VerEquals; else
	  if (cluster[1] == "<")
	    verDir = VerLess; else 
	    if (cluster[1] == ">")
	      verDir = VerGreater; else
	      {
		assert(0);
	      }
    userTask.itemsToInstall.push_back(UserTaskItemToInstall(cluster[0], verDir, cluster[2]));
  }

public:
  UserTask userTask;
}; //class DsInstallCliParser;

static DsInstallCliParser cliParser;

void printUrls(const TransactionIterator& it)
{
  StringVector install, upgrade, downgrade;
  const PkgVector& remove = it.getRemove();
  it.getUrls(install, upgrade, downgrade);
  std::cout << std::endl;
  if (!install.empty())
    {
      std::cout << "The following package(s) must be installed:" << std::endl << std::endl;
      for(StringVector::size_type i = 0;i < install.size();i++)
	std::cout << install[i] << std::endl;
      std::cout << std::endl;
    }
  if (!upgrade.empty())
    {
      std::cout << "The following package(s) must be upgraded:" << std::endl << std::endl;
      for(StringVector::size_type i = 0;i < upgrade.size();i++)
	std::cout << upgrade[i] << std::endl;
      std::cout << std::endl;
    }
  if (!downgrade.empty())
    {
      std::cout << "The following package(s) must be downgraded:" << std::endl << std::endl;
      for(StringVector::size_type i = 0;i < downgrade.size();i++)
	std::cout << downgrade[i] << std::endl;
      std::cout << std::endl;
    }
  if (!remove.empty())
    {
      std::cout << "The following package(s) must be removed:" << std::endl;
      std::cout << "#" << std::endl;
      for(StringVector::size_type i = 0;i < remove.size();i++)
	std::cout << "# " << remove[i].name << std::endl;
      std::cout << std::endl;
    }
  std::cout << "#" << install.size() << " package(s) to install, " << 
    upgrade.size() << " package(s) to upgrade, " <<
    downgrade.size() << " package(s) to downgrade," << 
    remove.size() << " package(s) to remove" << std::endl;
}

void printFiles(const TransactionIterator& it)
{
  const StringVector& install = it.getFilesInstall();
  const PkgVector& remove = it.getRemove();
  const StringToStringMap& upgrade = it.getFilesUpgrade();
  const StringToStringMap& downgrade = it.getFilesDowngrade();
  std::cout << std::endl;
  if (!install.empty())
    {
      std::cout << "The following package(s) must be installed:" << std::endl << std::endl;
      for(StringVector::size_type i = 0;i < install.size();i++)
	std::cout << install[i] << std::endl;
      std::cout << std::endl;
    }
  if (!upgrade.empty())
    {
      std::cout << "The following package(s) must be upgraded:" << std::endl << std::endl;
      for(StringToStringMap::const_iterator i = upgrade.begin();i != upgrade.end();i++)
	std::cout << i->second << std::endl;
      std::cout << std::endl;
    }
  if (!downgrade.empty())
    {
      std::cout << "The following package(s) must be downgraded:" << std::endl << std::endl;
      for(StringToStringMap::const_iterator i = downgrade.begin();i != downgrade.end();i++)
	std::cout << i->second << std::endl;
      std::cout << std::endl;
    }
  if (!remove.empty())
    {
      std::cout << "The following package(s) must be removed:" << std::endl;
      std::cout << "#" << std::endl;
      for(StringVector::size_type i = 0;i < remove.size();i++)
	std::cout << "# " << remove[i].name << std::endl;
      std::cout << std::endl;
    }
  std::cout << "#" << install.size() << " package(s) to install, " << 
    upgrade.size() << " package(s) to upgrade, " <<
    downgrade.size() << " package(s) to downgrade," << 
    remove.size() << " package(s) to remove" << std::endl;
}

void parseCmdLine(int argc, char* argv[])
{
  Messages(std::cout).dsInstallInitCliParser(cliParser);
  try {
    cliParser.init(argc, argv);
    cliParser.parse();
  }
  catch (const CliParserException& e)
    {
      switch (e.getCode())
	{
	case CliParserException::NoPrgName:
	  Messages(std::cerr).onMissedProgramName();
	  exit(EXIT_FAILURE);
	case CliParserException::MissedArgument:
	  Messages(std::cout).onMissedCommandLineArgument(e.getArg());
	  exit(EXIT_FAILURE);
	default:
	  assert(0);
	} //switch();
    }
  if (cliParser.wasKeyUsed("--help"))
    {
      Messages(std::cout).dsInstallHelp(cliParser);
      exit(EXIT_SUCCESS);
    }
}



int main(int argc, char* argv[])
{
  messagesProgramName = "ds-install";
  setlocale(LC_ALL, "");
  parseCmdLine(argc, argv);
  initLogging(cliParser.wasKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.wasKeyUsed("--log"));
  try{
    TransactionProgress transactionProgress(std::cout, cliParser.wasKeyUsed("--log"));
    ConfigCenter conf;
    conf.loadFromFile(DEFAULT_CONFIG_FILE_NAME);
    conf.loadFromDir(DEFAULT_CONFIG_DIR_NAME);
    conf.commit();
    if (!cliParser.wasKeyUsed("--log"))
      Messages(std::cout).dsInstallLogo();
    OperationCore core(conf);
    if (cliParser.userTask.itemsToInstall.empty())
      {
	Messages(std::cerr).onNoPackagesMentionedError();
	return EXIT_FAILURE;
      }
    if (!cliParser.wasKeyUsed("--sat"))
      {
	TransactionIterator::Ptr it = core.transaction(transactionProgress, cliParser.userTask);
	if (cliParser.wasKeyUsed("--urls"))
	  {
	    printUrls(*it.get());
	    return EXIT_SUCCESS;
	  }
	if (!cliParser.wasKeyUsed("--files"))
	  {
	    PackageListPrinting(conf).printSolution(*it.get());
	    std::cout << std::endl;
	  }
	if (it->emptyTask() || cliParser.wasKeyUsed("--dry-run"))
	  return EXIT_SUCCESS;
	if (!cliParser.wasKeyUsed("--files"))
	  if (!Messages(std::cout).confirmContinuing())
	    return EXIT_SUCCESS;
	std::cout << std::endl;
	FilesFetchProgress progress(std::cout, cliParser.wasKeyUsed("--log"));
	AlwaysTrueContinueRequest continueRequest;
	it->fetchPackages(progress, continueRequest);
	if (cliParser.wasKeyUsed("--files"))
	  {
	    printFiles(*it.get());
	    return EXIT_SUCCESS;
	  }
	it->makeChanges();
      } else
      {
	std::cout << std::endl;
	core.generateSat(transactionProgress, cliParser.userTask, std::cout);
      }
  }
  catch(const AbstractException& e)
    {
      ExceptionMessagesEn messages;
      e.accept(messages);
      std::cerr << messages.getMsg();
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
