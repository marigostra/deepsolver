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
#include"deepsolver/OsIntegrity.h"

using namespace Deepsolver;

int test(const ConfigCenter& conf)
{
  AbstractPkgBackEnd::Ptr backend = CREATE_PKG_BACKEND;
  OsIntegrity integrity(*backend.get());
  OperationCore core(conf);
  StringVector names;
  core.getPkgNames(names);
  logMsg(LOG_INFO, "test:%zu names obtained", names.size());
  for(StringVector::size_type i = 0;i < names.size();++i)
    {
      logMsg(LOG_INFO, "test:checking \'%s\':%zu of %zu", names[i].c_str(), i, names.size());
      UserTaskItemToInstallVector toInstall;
      toInstall.push_back(UserTaskItemToInstall(names[i]));
      PkgVector pkgs;
      core.closure(toInstall, pkgs);
      logMsg(LOG_INFO, "test:%zu packages are suggested to install", pkgs.size());
      if (!integrity.verify(pkgs))
	return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}

int main()
{
  initLogging(LOG_INFO, 1);
  try {
  ConfigCenter conf;
    conf.loadFromFile(DEFAULT_CONFIG_FILE_NAME);
    conf.loadFromDir(DEFAULT_CONFIG_DIR_NAME);
    conf.commit();
    return test(conf);
  }
  catch(const AbstractException& e)
    {
      std::cerr << e.getType() << " error:" << e.getMessage() << std::endl;
      return EXIT_FAILURE;
    }
}
