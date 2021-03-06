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
#include"deepsolver/CliParser.h"
#include"deepsolver/OperationCore.h"
#include"deepsolver/ExceptionMessagesEn.h"
#include"Messages.h"
#include"FilesFetchProgress.h"
#include"AlwaysTrueContinueRequest.h"

using namespace Deepsolver;

static CliParser cliParser;

void parseCmdLine(int argc, char* argv[])
{
  Messages(std::cout).dsUpdateInitCliParser(cliParser);
  try {
    cliParser.init(argc, argv);
    cliParser.parse();
  }
  catch (const CliParserException& e)
    {
      ExceptionMessagesEn messages;
      e.accept(messages);
      exit(EXIT_FAILURE);
    }
  if (cliParser.isKeyUsed("--help"))
    {
      Messages(std::cout).dsUpdateHelp(cliParser);
      exit(EXIT_SUCCESS);
    }
}

int main(int argc, char* argv[])
{
  setlocale(LC_ALL, "");
  parseCmdLine(argc, argv);
  initLogging(cliParser.isKeyUsed("--debug")?LOG_DEBUG:LOG_INFO, cliParser.isKeyUsed("--log"));
  try{
    AlwaysTrueContinueRequest alwaysTrueContinueRequest;
    if (!cliParser.isKeyUsed("--log"))
      Messages(std::cout).dsUpdateLogo();
    ConfigCenter conf;
    conf.loadFromFile(DEFAULT_CONFIG_FILE_NAME);
    conf.loadFromDir(DEFAULT_CONFIG_DIR_NAME);
    conf.commit();
    if (!cliParser.isKeyUsed("--log"))
      Messages(std::cout).introduceRepoSet(conf);
    OperationCore core(conf);
    FilesFetchProgress progress(std::cout, cliParser.isKeyUsed("--log"));
    core.fetchMetadata(progress, alwaysTrueContinueRequest);
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
