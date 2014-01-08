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

DEEPSOLVER_BEGIN_NAMESPACE

const ConfigFileException::Descr ConfigFileException::m_descr[] = {
  {SectionInvalidType, "only first level sections can be used with argument"},
  {SectionWaitingOpenBracket, "expecting the open bracket"},
  {SectionWaitingName, "expecting the section name"},
  {SectionInvalidNameChar, "an invalid character in section name or an unexpected end"},
  {SectionWaitingCloseBracketOrArg, "expecting the close bracket or the section argument"},
  {SectionUnexpectedArgEnd, "unexpected the section argument end"},
  {SectionWaitingCloseBracket, "expecting the close bracket"},
  {ValueWaitingName, "expecting the parameter name"},
  {ValueInvalidNameChar, "an invalid parameter name character or an unexpected end"},
  {ValueWaitingAssignOrNewName, "expecting an assignment or the new level name"},
  {ValueWaitingNewName, "expecting the new level name"},
  {ValueUnexpectedValueEnd, "an unexpected value end"},
  {-1, NULL}
};

const ConfigException::Descr ConfigException::m_descr[] = {
  {UnknownOption, "an unknown configuration option"},
  {ValueCannotBeEmpty, "the value can not be empty"},
  {AddingNotPermitted, "an adding is not permitted"},
  {InvalidBooleanValue, "an invalid boolean value"},
  {InvalidIntValue, "an invalid integer value"},
  {InvalidUIntValue, "an invalid unsigned integer value"},
  {InvalidUrl, "an invalid URL"},
  {-1, NULL}
};

std::string SystemException::getMessage() const
{
  if (m_comment.empty())
    return getDescr();
  return m_comment + ":" + getDescr();
}

std::string TaskException::getMessage() const
{
  switch(m_code)
    {
    case UnknownPkg:
      return "an unknown package name: " + m_param;
    case Contradiction:
      return "the task requires \'" + m_param + "\' to be installed and removed simultaneously";
    case UnsolvableSat:
      return "the task has no solutions";
    case Unmet:
      return "no packages matching the require entry:" + m_param;
    default:
      assert(0);
      return "";//Just to reduce warning 
    }; //switch(m_code);
}

std::string OperationCoreException::getMessage() const
{
  switch (m_code)
    {
    case InvalidInfoFile:
      return "the repository info file from \'" + m_param + "\' has an invalid content";
    case InvalidChecksumData:
      return "the checksum data format in \'" + m_param + "\' is invalid";
    case BrokenIndexFile:
      return "checksum verification failed for the file obtained from \'" + m_param + "\'";
    case LimitExceeded:
      return "the resource \'" + m_param + "\' is limited and its available amount is exceeded";
    case InvalidInstalledPkg:
      return "the installed package " + m_param + " is broken";
    case InvalidRepoPkg:
      return "the package " + m_param + " available through the attached repositories is broken";
    default:
      assert(0);
    } //switch(m_code);
  return "";
}

std::string IndexCoreException::getMessage() const
{
  switch(m_code)
    {
    case DirectoryNotEmpty:
      return "directory \'" + m_param + "\' is not empty";
    case CorruptedFile:
      return "file \'" + m_param + "\' is corrupted (wrong checksum)";
    case MissedChecksumFileName:
      return "the repository info file has no entry with checksum file name or it is empty";
    default:
      assert(0);
    } //switch(m_code);
  return "";
}

std::string ConfigFileException::getMessage() const
{
  std::ostringstream ss;
  ss << m_fileName << ":" << m_lineNumber << ":" << getDescr(m_code);
  return ss.str();
}

std::string ConfigException::getMessage() const
{
  const std::string location = getLocationDesignation();
  const std::string path = getOptionDesignation();
  const std::string msg = getDescr(m_code);
  if (!location.empty())
    return location + ":" + msg;
  if (!path.empty())
    return path + ":" + msg;
  return msg;
}

std::string InfoFileSyntaxException::getMessage() const
{
  std::ostringstream ss;
  ss << "line " << m_lineNumber << ":";
  switch(m_code)
    {
    case UnexpectedCharacter:
      ss << "an unexpected character";
      break;
    case IncompleteLine:
      ss << "an incomplete line";
      break;
    default:
      assert(0);
    } //switch(m_code);
  ss << ":" << m_line;
  return ss.str();
}

std::string InfoFileValueException::getMessage() const
{
  std::string s = "Info file value error:";
  switch(m_code)
    {
    case InvalidFormatType:
      return s + "invalid format type: \'" + m_param + "\'";
    case InvalidCompressionType:
      return s + "invalid compression type: \'" + m_param + "\'";
    case InvalidBooleanValue:
      return s + "parameter \'" + m_param + "\' has an invalid boolean value";
    default:
      assert(0);
    }
  return "";
}

std::string GzipException::getMessage() const
{
  return m_msg;
}

std::string CurlException::getMessage() const
{
  return m_url + ":" + m_descr;
}

std::string RegExpException::getMessage() const
{
  return "Invalid regular expression:" + m_message;
}

std::string Md5FileException::getMessage() const
{
  std::string msg;
  switch(m_code)
    {
    case TooShortLine:
      msg = "too short line";
      break;
    case InvalidChecksumFormat:
      msg = "invalid checksum format";
      break;
    default:
      assert(0);
    };
  std::ostringstream ss;
  ss << m_fileName << "(" << m_lineNumber << "):" << msg << ":" << m_line;
  return ss.str();
}

std::string PkgBackEndException::getMessage() const
{
  return m_fnName;
}

DEEPSOLVER_END_NAMESPACE
