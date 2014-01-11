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
#include"deepsolver/ExceptionMessagesEn.h"
#include"TransactionProgress.h"
#include"Messages.h"
#include"CliParser.h"
#include"PackageListPrinting.h"
#include"AlwaysTrueContinueRequest.h"
#include"FilesFetchProgress.h"

using namespace Deepsolver;

static CliParser cliParser;

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
  Messages(std::cout).dsRemoveInitCliParser(cliParser);
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
      Messages(std::cout).dsRemoveHelp(cliParser);
      exit(EXIT_SUCCESS);
    }
}

int doMainWork()
{
  TransactionProgress transactionProgress(std::cout, cliParser.wasKeyUsed("--log"));
  ConfigCenter conf;
  conf.loadFromFile(DEFAULT_CONFIG_FILE_NAME);
  conf.loadFromDir(DEFAULT_CONFIG_DIR_NAME);
  conf.commit();
  if (!cliParser.wasKeyUsed("--log"))
    Messages(std::cout).dsRemoveLogo();
  OperationCore core(conf);
  UserTask userTask;
  for(StringVector::size_type i = 0;i < cliParser.files.size();i++)
    if (!cliParser.files[i].empty())
      userTask.namesToRemove.insert(cliParser.files[i]);
  if (userTask.namesToRemove.empty())
    {
      Messages(std::cerr).onNoPackagesMentionedError();
      return EXIT_FAILURE;
    }
  if (!cliParser.wasKeyUsed("--sat"))
    {
      TransactionIterator::Ptr it = core.transaction(transactionProgress, userTask);
      if (cliParser.wasKeyUsed("--urls"))
	{
	  printUrls(*it.get());
	  return EXIT_SUCCESS;
	}
      if (!cliParser.wasKeyUsed("--files"))
	PackageListPrinting(conf).printSolution(*it.get(), cliParser.wasKeyUsed("--log"));
      if (it->emptyTask() || cliParser.wasKeyUsed("--dry-run"))
	return EXIT_SUCCESS;
      if (!cliParser.wasKeyUsed("--files"))
	if (!Messages(std::cout).confirmContinuing())
	  return 0;
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
      core.generateSat(transactionProgress, userTask, std::cout);
    }
  return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
  messagesProgramName = "ds-remove";
  setlocale(LC_ALL, "");
  parseCmdLine(argc, argv);
  initLogging(cliParser.wasKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.wasKeyUsed("--log"));
  try{
    return doMainWork();
  }
  catch(const AbstractException& e)
    {
      if (!cliParser.wasKeyUsed("--log"))
	{
	  ExceptionMessagesEn messages;
	  e.accept(messages);
	  std::cerr << messages.getMsg();
	} else
	std::cerr << e.getType() << " error:" << e.getMessage() << std::endl;
      return EXIT_FAILURE;
    }
}
