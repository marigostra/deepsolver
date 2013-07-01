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

static bool parseBooleanValue(const StringVector& path,
			      const std::string& sectArg,
			      const std::string& str,
			      const ConfigFilePosInfo& pos);

static unsigned int parseUIntValue(const StringVector& path,
				   const std::string& sectArg,
				   const std::string& str,
				   const ConfigFilePosInfo& pos);

static int parseIntValue(const StringVector& path,
			 const std::string& sectArg,
			 const std::string& str,
			 const ConfigFilePosInfo& pos);

void ConfigAdapter::checkValues()
{
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (!m_stringValues[i].canBeEmpty && trim(*m_stringValues[i].value).empty())
      throwConfigException(ConfigException::ValueCannotBeEmpty, m_stringValues[i]);
  for(StringListValueVector::size_type i = 0;i < m_stringListValues.size();i++)
    if (!m_stringListValues[i].canBeEmpty && (*m_stringListValues[i].value).empty())
      throwConfigException(ConfigException::ValueCannotBeEmpty, m_stringListValues[i]);
}

void ConfigAdapter::onValue(const StringVector& path, 
			    const std::string& sectArg,
			    const std::string& value,
			    bool adding,
			    const ConfigFilePosInfo& pos)
{
  assert(!path.empty());
  const int paramType = getType(path, sectArg, pos);
  switch(paramType)
    {
    case ValueTypeString:
      processStringValue(path, sectArg, value, adding, pos);
      break;
    case ValueTypeStringList:
      processStringListValue(path, sectArg, value, adding, pos);
      break;
    case ValueTypeBoolean:
      processBooleanValue(path, sectArg, value, adding, pos);
      break;
    case ValueTypeUInt:
      processUIntValue(path, sectArg, value, adding, pos);
      break;
    case ValueTypeInt:
      processIntValue(path, sectArg, value, adding, pos);
      break;
    default:
      assert(0);
    }
}

void ConfigAdapter::processStringValue(const StringVector& path, 
				       const std::string& sectArg,
				       const std::string& value,
				       bool adding,
				       const ConfigFilePosInfo& pos)
{
  const StringValueVector::size_type index =   findStringValue(path, sectArg);
  assert(index < m_stringValues.size());
  StringValue& v = m_stringValues[index];
  assert(v.value);
  if (!adding)
    (*v.value) = value; else
    (*v.value) += value;
  v.line = pos.line;
  v.fileName = pos.fileName;
  v.lineNumber = pos.lineNumber;
}

void ConfigAdapter::processStringListValue(const StringVector& path, 
					   const std::string& sectArg,
					   const std::string& value,
					   bool adding,
					   const ConfigFilePosInfo& pos)
{
  const StringListValueVector::size_type index = findStringListValue(path, sectArg);
  assert(index < m_stringListValues.size());
  StringListValue& v = m_stringListValues[index];
  assert(v.value);
  StringVector& l = *(v.value);
  if (!adding)
    l.clear();
  std::string item;
  for(std::string::size_type i = 0;i < value.size();i++)
    {
      std::string::size_type p = 0;
      while(p < v.delimiters.size() && value[i] != v.delimiters[p])
	p++;
      if (p >= v.delimiters.size())
	{
	  item += value[i];
	  continue;
	}
      if (!v.canContainEmptyItem && item.empty())
	throw ConfigException(ConfigException::ValueCannotBeEmpty, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
      l.push_back(item);
      item.erase();
    }
  if (!v.canContainEmptyItem && item.empty())
    throw ConfigException(ConfigException::ValueCannotBeEmpty, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
  l.push_back(item);
  v.line = pos.line;
  v.fileName = pos.fileName;
  v.lineNumber = pos.lineNumber;
}

void ConfigAdapter::processBooleanValue(const StringVector& path, 
					const std::string& sectArg,
					const std::string& value,
					bool adding,
					const ConfigFilePosInfo& pos)
{
  const BooleanValueVector::size_type index = findBooleanValue(path, sectArg);
  assert(index < m_booleanValues.size());
  BooleanValue& v =m_booleanValues[index]; 
  assert(v.value);
  bool& b = *(v.value);
  if (adding)
    throw ConfigException(ConfigException::AddingNotPermitted, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
  b = parseBooleanValue(path, sectArg, trim(value), pos);
  v.line = pos.line;
  v.fileName = pos.fileName;
  v.lineNumber = pos.lineNumber;
}

void ConfigAdapter::processUIntValue(const StringVector& path, 
				     const std::string& sectArg,
				     const std::string& value,
				     bool adding,
				     const ConfigFilePosInfo& pos)
{
  const UIntValueVector::size_type index = findUIntValue(path, sectArg);
  assert(index < m_uintValues.size());
  UIntValue& v = m_uintValues[index];
  assert(v.value);
  unsigned int& i = *(v.value);
  if (adding)
    throw ConfigException(ConfigException::AddingNotPermitted, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
  i = parseUIntValue(path, sectArg, value, pos);
  v.line = pos.line;
  v.fileName = pos.fileName;
  v.lineNumber = pos.lineNumber;
}

void ConfigAdapter::processIntValue(const StringVector& path, 
				     const std::string& sectArg,
				     const std::string& value,
				     bool adding,
				     const ConfigFilePosInfo& pos)
{
  const IntValueVector::size_type index = findIntValue(path, sectArg);
  assert(index < m_intValues.size());
  IntValue& v = m_intValues[index];
  assert(v.value);
  int& i = *(v.value);
  if (adding)
    throw ConfigException(ConfigException::AddingNotPermitted, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
  i = parseIntValue(path, sectArg, value, pos);
  v.line = pos.line;
  v.fileName = pos.fileName;
  v.lineNumber = pos.lineNumber;
}


int ConfigAdapter::getType(const StringVector& path, const std::string& sectArg, const ConfigFilePosInfo& pos) const
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
  for(UIntValueVector::size_type i = 0;i < m_uintValues.size();i++)
    if (m_uintValues[i].pathMatches(path, sectArg))
      return ValueTypeUInt;
  throw ConfigException(ConfigException::UnknownOption, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
}

ConfigAdapter::StringValueVector::size_type ConfigAdapter::findStringValue(const StringVector& path, const std::string& sectArg) const
{
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (m_stringValues[i].pathMatches(path, sectArg))
      return i;
  assert(0);
  return -1;
}

ConfigAdapter::StringListValueVector::size_type ConfigAdapter::findStringListValue(const StringVector& path, const std::string& sectArg) const
{
  for(StringListValueVector::size_type i = 0;i < m_stringListValues.size();i++)
    if (m_stringListValues[i].pathMatches(path, sectArg))
      return i;
  assert(0);
  return -1;
}

ConfigAdapter::BooleanValueVector::size_type ConfigAdapter::findBooleanValue(const StringVector& path, const std::string& sectArg) const
{
  for(BooleanValueVector::size_type i = 0;i < m_booleanValues.size();i++)
    if (m_booleanValues[i].pathMatches(path, sectArg))
      return i;
  assert(0);
  return -1;
}

ConfigAdapter::UIntValueVector::size_type ConfigAdapter::findUIntValue(const StringVector& path, const std::string& sectArg) const
{
  for(UIntValueVector::size_type i = 0;i < m_uintValues.size();i++)
    if (m_uintValues[i].pathMatches(path, sectArg))
      return i;
  assert(0);
  return -1;
}

ConfigAdapter::UIntValueVector::size_type ConfigAdapter::findIntValue(const StringVector& path, const std::string& sectArg) const
{
  for(IntValueVector::size_type i = 0;i < m_intValues.size();i++)
    if (m_intValues[i].pathMatches(path, sectArg))
      return i;
  assert(0);
  return -1;
}

void ConfigAdapter::throwConfigException(int code, const Value& value) const
{
  throw ConfigException(code, value.path, value.sectArg, value.line, value.fileName, value.lineNumber);
}

//Static functions;

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
  throw ConfigException(ConfigException::InvalidBooleanValue, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
}

unsigned int parseUIntValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
		       const ConfigFilePosInfo& pos)
{
  if (trim(str).empty())
    throw ConfigException(ConfigException::InvalidUIntValue, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
  std::istringstream ss(trim(str));
  unsigned int k;
  if (!(ss >> k))
    throw ConfigException(ConfigException::InvalidUIntValue, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
  return k;
}

int parseIntValue(const StringVector& path,
		       const std::string& sectArg,
		       const std::string& str,
		       const ConfigFilePosInfo& pos)
{
  if (trim(str).empty())
    throw ConfigException(ConfigException::InvalidIntValue, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
  std::istringstream ss(trim(str));
  int k;
  if (!(ss >> k))
    throw ConfigException(ConfigException::InvalidIntValue, path, sectArg, pos.line, pos.fileName, pos.lineNumber);
  return k;
}

DEEPSOLVER_END_NAMESPACE
