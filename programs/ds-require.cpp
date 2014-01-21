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

using namespace Deepsolver;

static CliParser cliParser;
static NamedPkgRel rel;

void parseCmdLine(int argc, char* argv[])
{
  Messages(std::cout).dsRequireInitCliParser(cliParser);
  try {
    cliParser.init(argc, argv);
    cliParser.parse();
  }
  catch (const CliParserException& e)
    {
      std::cerr << "command line error:" << e.getMessage() << std::endl;
      exit(EXIT_FAILURE);
    }
  if (cliParser.isKeyUsed("--help"))
    {
      Messages(std::cout).dsRequireHelp(cliParser);
      exit(EXIT_SUCCESS);
    }
  //  std::cout << cliParser.files.size() << std::endl;
  if (cliParser.files.size() != 1 && cliParser.files.size() != 3)
    {
      Messages(std::cerr).dsRequireOnInvalidInput();
      exit(EXIT_FAILURE);
    }
  rel.pkgName = cliParser.files[0];
  if (cliParser.files.size() == 3)
    {
      const std::string dir = cliParser.files[1];
      //      std::cout << dir << std::endl;
      if (dir == "<")
	rel.type = VerLess; else
	if (dir == "<=")
	  rel.type = VerLess | VerEquals; else
	  if (dir == "=")
	    rel.type = VerEquals; else
	    if (dir == ">=")
	      rel.type = VerEquals | VerGreater; else
	      if (dir == ">")
		rel.type = VerGreater; else
		{
		  Messages(std::cerr).dsRequireOnInvalidInput();
		  exit(EXIT_FAILURE);
		}
      rel.ver = cliParser.files[2];
      //      std::cout << "here" << std::endl;
    }
  if (rel.pkgName.empty() || (rel.type != VerNone && rel.ver.empty()))
    {
      Messages(std::cerr).dsRequireOnInvalidInput();
      exit(EXIT_FAILURE);
    }
}

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  parseCmdLine(argc, argv);
  initLogging(cliParser.isKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.isKeyUsed("--log"));
  try{
    ConfigCenter conf;
    conf.loadFromFile(DEFAULT_CONFIG_FILE_NAME);
    conf.loadFromDir(DEFAULT_CONFIG_DIR_NAME);
    conf.commit();
    OperationCore core(conf);
    core.printPackagesByRequire(rel, std::cout);
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
