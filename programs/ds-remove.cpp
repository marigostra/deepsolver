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

int main(int argc, char* argv[])
{
  messagesProgramName = "ds-remove";
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
	std::auto_ptr<TransactionIterator> it = core.transaction(transactionProgress, userTask);
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
	const std::string res = core.generateSat(transactionProgress, userTask);
	std::cout << std::endl;
	std::cout << res;
      }
  }
  catch (const ConfigFileException& e)
    {
      Messages(std::cerr).onConfigSyntaxError(e);
      return EXIT_FAILURE;
    }
  catch (const ConfigException& e)
    {
      Messages(std::cerr).onConfigError(e);
      return EXIT_FAILURE;
    }
  catch(const TaskException& e)
    {
       Messages(std::cerr).onTaskError(e);
       return EXIT_FAILURE;
    }
  catch(const OperationException& e)
    {
      Messages(std::cerr).onOperationError(e);
      return EXIT_FAILURE;
    }
  catch(const CurlException& e)
    {
      Messages(std::cerr).onCurlError(e);
      return EXIT_FAILURE;
    }
  catch(const PackageBackEndException& e)
    {
      Messages(std::cerr).onPackageBackEndError(e);
      return EXIT_FAILURE;
    }
  catch(const SystemException& e)
    {
      Messages(std::cerr).onSystemError(e);
      return EXIT_FAILURE;
    }
  catch(const NotImplementedException& e)
    {
      std::cerr << "Feature not implemented:" << e.getMessage() << std::endl;
      return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
