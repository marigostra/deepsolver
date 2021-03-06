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
#include"Messages.h"

using namespace Deepsolver;

namespace 
{
  CliParser cliParser;
  bool withInstalled = 0;
  bool withIds = 0;
}

void parseCmdLine(int argc, char* argv[])
{
  Messages(std::cout).dsSnapshotInitCliParser(cliParser);
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
      Messages(std::cout).dsSnapshotHelp(cliParser);
      exit(EXIT_SUCCESS);
    }
  withInstalled = cliParser.isKeyUsed("--installed");
  withIds = cliParser.isKeyUsed("--ids");
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
    core.printSnapshot(withInstalled, withIds, std::cout);
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
