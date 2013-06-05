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
#include"deepsolver/ConfigAdapter.h"

DEEPSOLVER_BEGIN_NAMESPACE

static std::string buildConfigParamTitle(const StringVector& path, const std::string& sectArg);
static bool parseBooleanValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
		       const ConfigFilePosInfo& pos);
static unsigned int parseUintValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
				   const ConfigFilePosInfo& pos);

void ConfigAdapter::commit()
{
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (!m_stringValues[i].canBeEmpty && trim(*m_stringValues[i].value).empty())
      throw ConfigException(ConfigException::ValueCannotBeEmpty, m_stringValues[i].pathToString());
  for(StringListValueVector::size_type i = 0;i < m_stringListValues.size();i++)
    if (!m_stringListValues[i].canBeEmpty && (*m_stringListValues[i].value).empty())
      throw ConfigException(ConfigException::ValueCannotBeEmpty, m_stringListValues[i].pathToString());
}

void ConfigAdapter::onValue(const StringVector& path, 
		       const std::string& sectArg,
		       const std::string& value,
				     bool adding,
				     const ConfigFilePosInfo& pos)
{
  assert(!path.empty());
  const int paramType = getParamType(path, sectArg, pos);
  if (paramType == ValueTypeString)
    {
      processStringValue(path, sectArg, value, adding, pos);
      return;
    }
  if (paramType == ValueTypeStringList)
    {
      processStringListValue(path, sectArg, value, adding, pos);
      return;
    }
  if (paramType == ValueTypeBoolean)
    {
      processBooleanValue(path, sectArg, value, adding, pos);
      return;
    }
  if (paramType == ValueTypeUint)
    {
      processUintValue(path, sectArg, value, adding, pos);
      return;
    }
  assert(0);
}

void ConfigAdapter::processStringValue(const StringVector& path, 
				      const std::string& sectArg,
				      const std::string& value,
				      bool adding,
				      const ConfigFilePosInfo& pos)
{
  StringValue stringValue;
  findStringValue(path, sectArg, stringValue);
  assert(stringValue.value != NULL);
  if (!adding)
    (*stringValue.value) = value; else
    (*stringValue.value) += value;
}

void ConfigAdapter::processStringListValue(const StringVector& path, 
				      const std::string& sectArg,
				      const std::string& value,
				      bool adding,
				      const ConfigFilePosInfo& pos)
{
  StringListValue stringListValue;
  findStringListValue(path, sectArg, stringListValue);
  assert(stringListValue.value != NULL);
  StringVector& v = *(stringListValue.value);
  if (adding)
    v.clear();
  std::string item;
  for(std::string::size_type i = 0;i < value.size();i++)
    {
      std::string::size_type p = 0;
      while(p < stringListValue.delimiters.size() && value[i] != stringListValue.delimiters[p])
	p++;
      if (p >= stringListValue.delimiters.size())
	{
	  item += value[i];
	  continue;
	}
      if (!stringListValue.canContainEmptyItem && trim(item).empty())
	throw ConfigException(ConfigException::ValueCannotBeEmpty, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
      v.push_back(item);
      item.erase();
    }
  if (!stringListValue.canContainEmptyItem && trim(item).empty())
    throw ConfigException(ConfigException::ValueCannotBeEmpty, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
  v.push_back(item);
}

void ConfigAdapter::processBooleanValue(const StringVector& path, 
				      const std::string& sectArg,
				      const std::string& value,
				      bool adding,
				      const ConfigFilePosInfo& pos)
{
  BooleanValue booleanValue;
  findBooleanValue(path, sectArg, booleanValue);
  assert(booleanValue.value != NULL);
  bool& v = *(booleanValue.value);
  if (adding)
    throw ConfigException(ConfigException::AddingNotPermitted, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
  v = parseBooleanValue(path, sectArg, trim(value), pos);
}

void ConfigAdapter::processUintValue(const StringVector& path, 
				      const std::string& sectArg,
				      const std::string& value,
				      bool adding,
				      const ConfigFilePosInfo& pos)
{
  UintValue uintValue;
  findUintValue(path, sectArg, uintValue);
  assert(uintValue.value != NULL);
  unsigned int& v = *(uintValue.value);
  v = parseUintValue(path, sectArg, value, pos);
}

int ConfigAdapter::getParamType(const StringVector& path, const std::string& sectArg, const ConfigFilePosInfo& pos) const
{
  //String;
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (m_stringValues[i].pathMatches(path, sectArg))
      return ValueTypeString;
  //StringList;
  for(StringListValueVector::size_type i = 0;i < m_stringListValues.size();i++)
    if (m_stringListValues[i].pathMatches(path, sectArg))
      return ValueTypeStringList;
  //Boolean;
  for(BooleanValueVector::size_type i = 0;i < m_booleanValues.size();i++)
    if (m_booleanValues[i].pathMatches(path, sectArg))
      return ValueTypeBoolean;
  //Unsigned integer;
  for(UintValueVector::size_type i = 0;i < m_uintValues.size();i++)
    if (m_uintValues[i].pathMatches(path, sectArg))
      return ValueTypeUint;
  throw ConfigException(ConfigException::UnknownParam, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
}

void ConfigAdapter::findStringValue(const StringVector& path, 
				   const std::string& sectArg,
				   StringValue& stringValue)
{
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (m_stringValues[i].pathMatches(path, sectArg))
      {
	stringValue = m_stringValues[i];
	return;
      }
  assert(0);
}

void ConfigAdapter::findStringListValue(const StringVector& path, 
				   const std::string& sectArg,
				   StringListValue& stringListValue)
{
  for(StringListValueVector::size_type i = 0;i < m_stringListValues.size();i++)
    if (m_stringListValues[i].pathMatches(path, sectArg))
      {
	stringListValue = m_stringListValues[i];
	return;
      }
  assert(0);
}

void ConfigAdapter::findBooleanValue(const StringVector& path, 
				   const std::string& sectArg,
				   BooleanValue& booleanValue)
{
  for(BooleanValueVector::size_type i = 0;i < m_booleanValues.size();i++)
    if (m_booleanValues[i].pathMatches(path, sectArg))
      {
	booleanValue = m_booleanValues[i];
	return;
      }
  assert(0);
}

void ConfigAdapter::findUintValue(const StringVector& path, 
				   const std::string& sectArg,
				   UintValue& uintValue)
{
  for(BooleanValueVector::size_type i = 0;i < m_uintValues.size();i++)
    if (m_uintValues[i].pathMatches(path, sectArg))
      {
	uintValue = m_uintValues[i];
	return;
      }
  assert(0);
}

//Static functions;

std::string buildConfigParamTitle(const StringVector& path, const std::string& sectArg)
{
  assert(!path.empty());
  std::string value = path[0];
  if (!sectArg.empty())
    value += " \"" + sectArg + "\"";
  for(StringVector::size_type i = 1;i < path.size();i++)
    value += "." + path[i];
  return value;
}

bool parseBooleanValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
		       const ConfigFilePosInfo& pos)
{
  if (str == "YES" || str == "Yes" || str == "yes")
    return 1;
  if (str == "TRUE" || str == "True" || str == "true")
    return 1;
  if (str == "1")
    return 1;
  if (str == "NO" || str == "No" || str == "no")
    return 0;
  if (str == "FALSE" || str == "False" || str == "false")
    return 0;
  if (str == "0")
    return 0;
  throw ConfigException(ConfigException::InvalidBooleanValue, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
}

unsigned int parseUintValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
		       const ConfigFilePosInfo& pos)
{
  if (trim(str).empty())
    throw ConfigException(ConfigException::InvalidUintValue, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
  std::istringstream ss(trim(str));
  unsigned int k;
  if (!(ss >> k))
    throw ConfigException(ConfigException::InvalidUintValue, buildConfigParamTitle(path, sectArg), pos.fileName, pos.lineNumber, pos.line);
  return k;
}

DEEPSOLVER_END_NAMESPACE
