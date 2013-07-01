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

#ifndef DEEPSOLVER_CONFIG_ADAPTER_H
#define DEEPSOLVER_CONFIG_ADAPTER_H

#include"deepsolver/ConfigFile.h"

namespace Deepsolver
{
  class ConfigAdapter
  {
  public:
    /**\brief The default constructor*/
    ConfigAdapter() {}

    /**\brief The destructor*/
    virtual ~ConfigAdapter() {}

  public:
    void checkValues();

  protected:
    void onValue(const StringVector& path, 
			   const std::string& sectArg,
			   const std::string& value,
			   bool adding,
			   const ConfigFilePosInfo& pos);

  protected:
    enum {ValueTypeString, ValueTypeStringList, ValueTypeBoolean, ValueTypeInt, ValueTypeUInt};

    class Value
    {
    public:
      Value() : lineNumber(0) {}

    public:
      bool pathMatches(const StringVector& p, const std::string& a) const 
      {
	if (path.size() != p.size())
	  return 0;
	for(StringVector::size_type i = 0;i < path.size();i++)
	  if (path[i] != p[i])
	    return 0;
	if (!sectArg.empty() && sectArg != a)
	  return 0;
	return 1;
      }

      std::string pathToString() const
      {
	if (path.empty())
	  return "";
	std::string res = path[0];
	if (!sectArg.empty())
	  res += " \"" + sectArg + "\"";
	for(StringVector::size_type i = 1;i < path.size();i++)
	  res += "." + path[i];
	return res;
      }

    public:
      StringVector path;
      std::string sectArg;
      std::string line;
      std::string fileName;
      size_t lineNumber;
    }; //class Value;

    class StringValue: public Value 
    {
    public:
      StringValue()
	: canBeEmpty(0),
	  value(NULL) {}

      StringValue(std::string& v)
	: canBeEmpty(0),
	  value(&v) {}

    public:
      bool canBeEmpty;
      std::string* value;
    }; //class StringValue;

    class StringListValue : public Value
    {
    public:
      StringListValue()
	: canContainEmptyItem(0),
	  canBeEmpty(0),
	  value(NULL) {}

      StringListValue(StringVector& v)
	: canContainEmptyItem(0),
	  canBeEmpty(0),
	  value(&v) {}

    public:
      bool canContainEmptyItem;
      bool canBeEmpty;
      std::string delimiters;
      StringVector* value;
    }; //class StringListValue;

    template<typename T>
    class ScalarValue: public Value
    {
    public:
      ScalarValue()
	: value(NULL) {}

      ScalarValue(T& v)
	: value(&v) {}

    public:
      T* value;
    }; //class ScalarValue;

  protected:
    typedef ScalarValue<bool> BooleanValue;
    typedef ScalarValue<unsigned int> UIntValue;
    typedef ScalarValue<int> IntValue;

  protected:
    typedef std::vector<StringValue> StringValueVector;
    typedef std::vector<StringListValue> StringListValueVector;
    typedef std::vector<BooleanValue> BooleanValueVector;
    typedef std::vector<UIntValue> UIntValueVector;
    typedef std::vector<IntValue> IntValueVector;

  protected:
    void throwConfigException(int code, const Value& value) const;

    int getType(const StringVector& path,
		const std::string& sectArg,
		const ConfigFilePosInfo& pos) const;

    void processStringValue(const StringVector& path, 
			    const std::string& sectArg,
			    const std::string& value,
			    bool adding,
			    const ConfigFilePosInfo& pos);

    void processStringListValue(const StringVector& path, 
				const std::string& sectArg,
				const std::string& value,
				bool adding,
				const ConfigFilePosInfo& pos);

    void processBooleanValue(const StringVector& path, 
			     const std::string& sectArg,
			     const std::string& value,
			     bool adding,
			     const ConfigFilePosInfo& pos);

    void processUIntValue(const StringVector& path, 
			  const std::string& sectArg,
			  const std::string& value,
			  bool adding,
			  const ConfigFilePosInfo& pos);

    void processIntValue(const StringVector& path, 
			  const std::string& sectArg,
			  const std::string& value,
			  bool adding,
			  const ConfigFilePosInfo& pos);

    StringValueVector::size_type findStringValue(const StringVector& path, const std::string& sectArg) const;
    StringListValueVector::size_type findStringListValue(const StringVector& path, const std::string& sectArg) const;
    BooleanValueVector::size_type findBooleanValue(const StringVector& path, const std::string& sectArg) const;
    UIntValueVector::size_type findUIntValue(const StringVector& path, const std::string& sectArg) const;
    IntValueVector::size_type findIntValue(const StringVector& path, const std::string& sectArg) const;

protected:
    StringValueVector m_stringValues;
    StringListValueVector m_stringListValues;
    BooleanValueVector m_booleanValues;
    UIntValueVector m_uintValues;
    IntValueVector m_intValues;
  }; //class ConfigAdapter;
} //namespace Deepsolver;

#endif //DEEPSOLVER_CONFIG_ADAPTER_H;
