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

DEEPSOLVER_BEGIN_NAMESPACE

void ExceptionMessagesEn::visit(const CliParserException& e)
{
  switch(e.getCode())
    {
    case CliParserException::NoPrgName:
      m_stream << "command line error:argv[0] doesn\'t contain the program name" << std::endl;
      return;
    case CliParserException::MissedArgument:
      m_stream << "command line error:the key \'" << e.getParam() << "\' requires the additional parameter" << std::endl;
      return;
    }
}

void ExceptionMessagesEn::visit(const SystemException& e)
{
  m_stream << "System error were occurred during last operation. That may be caused" << std::endl;
  m_stream << "by improper Deepsolver installation or by damaged environment." << std::endl;
  m_stream << "Here is problem details:" << std::endl;
  m_stream << std::endl;
  m_stream << e.getMessage() << std::endl;
  m_stream << std::endl;
}

void ExceptionMessagesEn::visit(const ConfigFileException& e)
{
  m_stream << "There is an error in your configuration file. Please, consult your" << std::endl;
  m_stream << "system administrator for problem resolving. Details:" << std::endl;
  m_stream << std::endl;
  m_stream << "ERROR:" << ConfigFileException::getDescr(e.getCode()) << std::endl;
  m_stream << std::endl;
  std::ostringstream ss;
  ss << e.getFileName() << ":" << e.getLineNumber() << ":";
  const size_t pos = ss.str().length() + e.getPos();//FIXME:UTF-8 character make this value incorrect;
  m_stream << ss.str() << e.getLine() << std::endl;
  for(size_t i = 0;i < pos;i++)
    m_stream << " ";
  m_stream << "^" << std::endl;
}

void ExceptionMessagesEn::visit(const ConfigException& e)
{
  m_stream << "There is an error in your configuration file. Please, fix and try again." << std::endl;
  m_stream << std::endl;
  m_stream << "Error: ";
  switch(e.getCode())
    {
    case ConfigException::UnknownOption:
      m_stream << "An unknown  option" << std::endl;
      break;
    case ConfigException::ValueCannotBeEmpty:
      m_stream << "Value cannot be empty" << std::endl;
      break;
    case ConfigException::AddingNotPermitted:
      m_stream << "Adding not permitted" << std::endl;
      break;
    case ConfigException::InvalidBooleanValue:
      m_stream << "An invalid boolean value" << std::endl;
      break;
    case ConfigException::InvalidUIntValue:
      m_stream << "An invalid unsigned integer value" << std::endl;
      break;
    case ConfigException::InvalidIntValue:
      m_stream << "An invalid integer value" << std::endl;
      break;
    case ConfigException::InvalidUrl:
      m_stream << "An invalid URL" << std::endl;
      break;
    default:
      assert(0);
      return;
    } //switch(e.getCode());
  m_stream << "Option: " << e.getOptionDesignation() << std::endl;
  if (e.getLineNumber() > 0)
    {
      m_stream <<"Location: " <<  e.getLocationDesignation() << std::endl;
      m_stream << "Line content: " << e.getLine() << std::endl;
    }
}

void ExceptionMessagesEn::visit(const CurlException& e)
{
  m_stream << "Your network connection is experiencing difficulties or you have the" << std::endl;
  m_stream << "error in your configuration files. Please, consult your system" << std::endl;
  m_stream << "administrator. Problem detailes:" << std::endl;
  m_stream << std::endl;
  m_stream << "URL: " << e.getUrl() << std::endl;
  m_stream << "ERROR: " << e.getDescr() << std::endl;
  m_stream << std::endl;
}

void ExceptionMessagesEn::visit(const PkgBackEndException& e)
{
  m_stream << "There is the problem in the package back-end layer. Description (very likely a failed function name) below:" << std::endl;
  m_stream << std::endl;
  m_stream << "ERROR:" << e.getMessage() << std::endl;
}

void ExceptionMessagesEn::visit(const OperationCoreException& e)
{
  m_stream << "The requested operation cannot be properly performed due to unexpected errors." << std::endl;
  switch(e.getCode())
    {
    case OperationCoreException::InvalidInfoFile:
      m_stream << "the information file for one or more repositories contains incorrect" << std::endl;
      m_stream << "data or damaged. Usually it means an invalid URL of remote repository is used" << std::endl;
      m_stream << "or repository provider is experiencing technical problems. Please, be" << std::endl;
      m_stream << "sure your configuration data is correct and try again later." << std::endl;
      m_stream << "Here is the URL of the incorrect info file:" << std::endl;
      m_stream << std::endl;
      m_stream << e.getParam() << std::endl;
      m_stream << std::endl;
      break;
    case OperationCoreException::InvalidChecksumData:
      m_stream << "One of the attached repositories has corrupted checksum file and it" << std::endl;
      m_stream << "cannot be properly parsed. Usually it means you have an error in your" << std::endl;
      m_stream << "configuration data or repository provider is experiencing technical" << std::endl;
      m_stream << "problems. Without checksum data Deepsolver is unable to be sure" << std::endl;
      m_stream << "the fetched files are not broken. Please, consult your system" << std::endl;
      m_stream << "administrator or try again later. Here is the URL of the invalid" << std::endl;
      m_stream << "checksum file:" << std::endl;
      m_stream << std::endl;
      m_stream << e.getParam() << std::endl;
      m_stream << std::endl;
      break;
    case OperationCoreException::BrokenIndexFile:
      m_stream << "one or more files in repository index is broken. Since basic repository data" << std::endl;
      m_stream << "is correct very likely it means the repository provider is experiencing technical" << std::endl;
      m_stream << "problems. Here is the URL of the corrupted file:" << std::endl;
      m_stream << std::endl;
      m_stream << e.getParam() << std::endl;
      m_stream << std::endl;
      break;
    case OperationCoreException::LimitExceeded:
      m_stream << "One of the defined limits was exceeded during last operation. Please" << std::endl;
      m_stream << "contact system administrator to resolve this situation." << std::endl;
    case OperationCoreException::InvalidInstalledPkg:
      m_stream << "Your system has one or more invalid packages. The work is now stopped" << std::endl;
      m_stream << "since it can cause potentially harmful situation. If you are sure that" << std::endl;
      m_stream << "it is not a real problem you can disable this check with" << std::endl;
      m_stream << "\'core.stop-invalid-installed-pkg\' option in your configuration file." << std::endl;
      break;
    case OperationCoreException::InvalidRepoPkg:
      m_stream << "The repositories you have attached in your configuration contain one or more invalid packages. The work is now stopped" << std::endl;
      m_stream << "since it can cause potentially harmful situation. If you are sure that" << std::endl;
      m_stream << "it is not a real problem you can disable this check with" << std::endl;
      m_stream << "\'core.stop-invalid-repo-pkg\' option in your configuration file." << std::endl;
      break;
    default:
      assert(0);
    } //switch(e.getCode());
}

void ExceptionMessagesEn::visit(const TaskException& e)
{
  switch(e.getCode())
    {
    case TaskException::UnknownPkg:
      m_stream << "You have requested the package with unknown name. The following name is not present neither in the system nor in the available repositories." << std::endl << std::endl;
      m_stream << "The unknown package name is:" << e.getParam() << std::endl;
      return;
    case TaskException::Contradiction:
      m_stream << "You have issued the meaningless request.  It takes the same package" << std::endl;
      m_stream << "for installation and removing simultaneously. This is impossible." << std::endl << std::endl;
      m_stream << "The packaged caused the problem:" << e.getParam() << std::endl;
      return;
    case TaskException::UnsolvableSat:
      m_stream << "Your request has no solution. That can be caused either by a contradiction" << std::endl;
      m_stream << "in the list of requested packages or by the problems in the attached repositories." << std::endl;
      return;
    case TaskException::Unmet:
      m_stream << "There is a require entry without any matching packages neither in the" << std::endl;
      m_stream << "system not in the available repositories. The entry caused the break is:" << std::endl << std::endl;
      m_stream << e.getParam() << std::endl;
      return;
    default:
      assert(0);
    };
}

DEEPSOLVER_END_NAMESPACE
