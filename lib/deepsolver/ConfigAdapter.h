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
    void commit();

  protected:
    void onValue(const StringVector& path, 
			   const std::string& sectArg,
			   const std::string& value,
			   bool adding,
			   const ConfigFilePosInfo& pos);

  protected:
    enum {
      ValueTypeString = 1,
      ValueTypeStringList = 2,
      ValueTypeBoolean = 3,
      ValueTypeUint = 4
    };

    class Value
    {
    public:
    public:
      Value() {}
      virtual ~Value() {}

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
	assert(!path.empty());
	std::string value = path[0];
	if (!sectArg.empty())
	  value += " \"" + sectArg + "\"";
	for(StringVector::size_type i = 1;i < path.size();i++)
	  value += "." + path[i];
	return value;
      }

    public:
      StringVector path;
      std::string sectArg;
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

      virtual ~StringValue() {}

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

      virtual ~StringListValue() {}

    public:
      bool canContainEmptyItem;
      bool canBeEmpty;
      std::string delimiters;
      StringVector* value;
    }; //class StringListValue;

    class BooleanValue: public Value
    {
    public:
      BooleanValue()
	: value(NULL) {}

      BooleanValue(bool& v)
	: value(&v) {}

      virtual ~BooleanValue() {}

    public:
      bool* value;
    }; //class BooleanValue;

    class UintValue: public Value
    {
    public:
      UintValue()
	: value(NULL) {}

      UintValue(unsigned int& v)
	: value(&v) {}

      virtual ~UintValue() {}

    public:
      unsigned int* value;
    }; //class UintValue;

  protected:
    int getParamType(const StringVector& path, const std::string& sectArg, const ConfigFilePosInfo& pos) const;

    void processStringValue(const StringVector& path, 
			    const std::string& sectArg,
			    const std::string& value,
			    bool adding,
			    const ConfigFilePosInfo& pos);

    void findStringValue(const StringVector& path, 
			 const std::string& sectArg,
			 StringValue& stringValue);

    void processStringListValue(const StringVector& path, 
				const std::string& sectArg,
				const std::string& value,
				bool adding,
				const ConfigFilePosInfo& pos);

    void findStringListValue(const StringVector& path, 
			     const std::string& sectArg,
			     StringListValue& stringListValue);

    void processBooleanValue(const StringVector& path, 
			     const std::string& sectArg,
			     const std::string& value,
			     bool adding,
			     const ConfigFilePosInfo& pos);

    void findBooleanValue(const StringVector& path, 
			  const std::string& sectArg,
			  BooleanValue& booleanValue);

    void processUintValue(const StringVector& path, 
			  const std::string& sectArg,
			  const std::string& value,
			  bool adding,
			  const ConfigFilePosInfo& pos);

    void findUintValue(const StringVector& path, 
		       const std::string& sectArg,
		       UintValue& uintValue);

protected:
    typedef std::vector<StringValue> StringValueVector;
    typedef std::vector<StringListValue> StringListValueVector;
    typedef std::vector<BooleanValue> BooleanValueVector;
    typedef std::vector<UintValue> UintValueVector;

protected:
    StringValueVector m_stringValues;
    StringListValueVector m_stringListValues;
    BooleanValueVector m_booleanValues;
    UintValueVector m_uintValues;
  }; //class ConfigAdapter;
} //namespace Deepsolver;

#endif //DEEPSOLVER_CONFIG_ADAPTER_H;
