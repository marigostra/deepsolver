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
#include"deepsolver/ExceptionMessagesEn.h"

using namespace Deepsolver;

void simpleReport(std::shared_ptr<AbstractException> e)
{
  std::cout << e->getType() << " error:" << e->getMessage() << std::endl;
}

void visitorReport(std::shared_ptr<AbstractException> e)
{
  ExceptionMessagesEn messages;
  e->accept(messages);
  std::cout << e->getType() << " error:" << messages.getMsg() << std::endl;
}


template<class T>
std::shared_ptr<T> createException(int code)
{
  return std::shared_ptr<T>(new T(code));
}

template<>
std::shared_ptr<SystemException> createException<SystemException>(int code)
{
  return std::shared_ptr<SystemException>(new SystemException(code, "/tmp"));
}

template<>
std::shared_ptr<TaskException> createException<TaskException>(int code)
{
  return std::shared_ptr<TaskException>(new TaskException(code, "foobar-1.0.0-alt1"));
}

template<>
std::shared_ptr<OperationCoreException> createException<OperationCoreException>(int code)
{
  return std::shared_ptr<OperationCoreException>(new OperationCoreException(code, "#PARAM#"));
}

template<>
std::shared_ptr<IndexCoreException> createException<IndexCoreException>(int code)
{
  return std::shared_ptr<IndexCoreException>(new IndexCoreException(code, "#PARAM#"));
}

template<>
std::shared_ptr<ConfigFileException> createException<ConfigFileException>(int code)
{
  return std::shared_ptr<ConfigFileException>(new ConfigFileException(code, "/tmp/ds.conf", 1, 1, "#LINE#"));
}

template<>
std::shared_ptr<ConfigException> createException<ConfigException>(int code)
{
  StringVector path;
  path.push_back("core");
  path.push_back("foobar");
  return std::shared_ptr<ConfigException>(new ConfigException(code, path, "", "foobar = 1", "/tmp/ds.conf", 1));
}

template<>
std::shared_ptr<InfoFileSyntaxException> createException<InfoFileSyntaxException>(int code)
{
  return std::shared_ptr<InfoFileSyntaxException>(new InfoFileSyntaxException(code, 1, "#LINE#"));
}

template<typename T>
void tests(T report)
{
  std::shared_ptr<AbstractException> e;
  //SystemException;
  e = createException<SystemException>(EPERM);
  report(e);
  //TaskException;
  for(size_t i = 0;i < TaskException::CodeCount;++i)
    {
      std::shared_ptr<TaskException> e = createException<TaskException>(i);
      report(e);
    }
  //OperationCoreException;
  for(size_t i = 0;i < OperationCoreException::CodeCount;++i)
    {
      std::shared_ptr<OperationCoreException> e = createException<OperationCoreException>(i);
      report(e);
    }
  //IndexCoreException;
  for(size_t i = 0;i < IndexCoreException::CodeCount;++i)
    {
      std::shared_ptr<IndexCoreException> e = createException<IndexCoreException>(i);
      report(e);
    }
  //ConfigFileException;
  for(size_t i = 0;i < ConfigFileException::CodeCount;++i)
    {
      std::shared_ptr<ConfigFileException> e = createException<ConfigFileException>(i);
      report(e);
    }
  //ConfigException;
  for(size_t i = 0;i < ConfigException::CodeCount;++i)
    {
      std::shared_ptr<ConfigException> e = createException<ConfigException>(i);
      report(e);
    }
  //InfoFileSyntaxException;
  for(size_t i = 0;i < InfoFileSyntaxException::CodeCount;++i)
    {
      std::shared_ptr<InfoFileSyntaxException> e = createException<InfoFileSyntaxException>(i);
      report(e);
    }
}

int main()
{
  tests([](std::shared_ptr<AbstractException> e){ simpleReport(e); });
  tests([](std::shared_ptr<AbstractException> e){ visitorReport(e); });
  return 0;
}
