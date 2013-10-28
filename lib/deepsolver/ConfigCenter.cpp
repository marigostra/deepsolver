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
#include"deepsolver/ConfigCenter.h"

#define DELIMITERS ";"

DEEPSOLVER_BEGIN_NAMESPACE

//static std::string buildConfigParamTitle(const StringVector& path, const std::string& sectArg);

static bool checkUrl(const std::string& url);



void ConfigCenter::initValues()
{

  addBooleanParam2("core", "stop-invalid-installed-pkg", m_root.stopOnInvalidInstalledPkg);
  addBooleanParam2("core", "stop-invalid-repo-pkg", m_root.stopOnInvalidRepoPkg);


  addNonEmptyStringParam3("core", "dir", "pkg-data", m_root.dir.pkgData);
  addNonEmptyStringParam3("core", "dir", "pkg-cache", m_root.dir.pkgCache);
  addStringListParam3("core", "os", "transact-read-ahead", m_root.os.transactReadAhead);
}

void ConfigCenter::initRepoValues()
{
  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
    {
      ConfRepo& repo = m_root.repo[i];
      StringValue stringValue;
      stringValue.path.push_back("repo");
      stringValue.sectArg = repo.name;
      //URL;
      stringValue.path.push_back("url");
      stringValue.value = &repo.url;
      stringValue.canBeEmpty = 0;
      m_stringValues.push_back(stringValue);
      //Vendor;
      stringValue.path[1] = "vendor";
      stringValue.value = &repo.vendor;
      stringValue.canBeEmpty = 1;
      m_stringValues.push_back(stringValue);
      //Arch;
      StringListValue stringListValue;
      stringListValue.sectArg = repo.name;
      stringListValue.path.push_back("repo");
      stringListValue.path.push_back("arch");
      stringListValue.delimiters = DELIMITERS;
      stringListValue.canContainEmptyItem = 0;
      stringListValue.canBeEmpty = 0;
      stringListValue.value = &repo.arch;
      m_stringListValues.push_back(stringListValue);
      //Components;
      stringListValue.path[1] = "components";
      stringListValue.value = &repo.components;
      m_stringListValues.push_back(stringListValue);
      //Enabled;
      BooleanValue booleanValue;
      booleanValue.sectArg = repo.name;
      booleanValue.path.push_back("repo");
      booleanValue.path.push_back("enabled");
      booleanValue.value = &repo.enabled;
      m_booleanValues.push_back(booleanValue);
      //Take*;
      booleanValue.path[1] = "take-descr";
      booleanValue.value = &repo.takeDescr;
      m_booleanValues.push_back(booleanValue);
      booleanValue.path[1] = "take-file-list";
      booleanValue.value = &repo.takeFileList;
      m_booleanValues.push_back(booleanValue);
      booleanValue.path[1] = "take-sources";
      booleanValue.value = &repo.takeSources;
      m_booleanValues.push_back(booleanValue);
    }
}

void ConfigCenter::initProvideValues()
{
  for(ConfProvideVector::size_type i = 0;i < m_root.provide.size();i++)
    {
      ConfProvide& provide = m_root.provide[i];
      StringListValue stringListValue;
      stringListValue.sectArg = provide.name;
      stringListValue.path.push_back("provide");
      stringListValue.path.push_back("providers");
      stringListValue.delimiters = DELIMITERS;
      stringListValue.canContainEmptyItem = 0;
      stringListValue.canBeEmpty = 1;
      stringListValue.value = &provide.providers;
      m_stringListValues.push_back(stringListValue);
    }
}

void ConfigCenter::commit()
{
  m_root.dir.pkgData = trim(m_root.dir.pkgData);
  for(StringVector::size_type i = 0;i < m_root.os.transactReadAhead.size();i++)
    m_root.os.transactReadAhead[i] = trim(m_root.os.transactReadAhead[i]);
  for(ConfProvideVector::size_type i = 0;i < m_root.provide.size();i++)
    {
      ConfProvide& provide = m_root.provide[i];
      provide.name = trim(provide.name);
      for(StringVector::size_type k = 0;k < provide.providers.size();k++)
	provide.providers[k] = trim(provide.providers[k]);
    }
  for(ConfRepoVector::size_type i = 0;i < m_root.repo.size();i++)
    {
      ConfRepo& repo = m_root.repo[i];
      repo.name = trim(repo.name);
      repo.url = trim(repo.url);
      repo.vendor = trim(repo.vendor);
      for(StringVector::size_type k = 0;k < repo.arch.size();k++)
	repo.arch[k] = trim(repo.arch[k]);
      for(StringVector::size_type k = 0;k < repo.components.size();k++)
	repo.components[k] = trim(repo.components[k]);
    }
  ConfigAdapter::checkValues();
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    if (m_stringValues[i].path.size() == 2 &&
	m_stringValues[i].path[0] == "repo" &&
	m_stringValues[i].path[1] == "url" &&
	!checkUrl(*m_stringValues[i].value))
      throwConfigException(ConfigException::InvalidUrl, m_stringValues[i]);
}

void ConfigCenter::printConfigData(std::ostream& s) const
{
  StringVector v;
  for(StringValueVector::size_type i = 0;i < m_stringValues.size();i++)
    v.push_back(m_stringValues[i].pathToString() + " = \"" + (*m_stringValues[i].value) + "\"");
  for(StringListValueVector::size_type i = 0;i < m_stringListValues.size();i++)
    {
      std::string k = m_stringListValues[i].pathToString() + " =";
      const StringVector& values = *m_stringListValues[i].value;
      for(StringVector::size_type j = 0;j < values.size();j++)
	{
	  k += " \"" + values[j] + "\"";
	  if (j + 1 < values.size())
	    k += ",";
	}
      v.push_back(k);
    }
  for(BooleanValueVector::size_type i = 0;i < m_booleanValues.size();i++)
    v.push_back(m_booleanValues[i].pathToString() + " = " + ((*m_booleanValues[i].value)?"yes":"no"));
  for(UIntValueVector::size_type i = 0;i < m_uintValues.size();i++)
    {
      std::ostringstream ss;
      ss << m_uintValues[i].pathToString() << " = " << (*m_uintValues[i].value);
      v.push_back(ss.str());
    }
  std::sort(v.begin(), v.end());
  for(StringVector::size_type i = 0;i < v.size();i++)
    s << v[i] << std::endl;
}

void ConfigCenter::onConfigFileValue(const StringVector& path, 
		       const std::string& sectArg,
		       const std::string& value,
				     bool adding,
				     const ConfigFilePosInfo& pos)
{
  assert(!path.empty());
  if (path[0] == "repo")
    {
      if (trim(sectArg).empty())
	throw NotImplementedException("Empty configuration file section argument");
      ConfRepoVector::size_type i = 0;
      while(i < m_root.repo.size() && m_root.repo[i].name != sectArg)
	i++;
      if (i >= m_root.repo.size())
	{
	  m_root.repo.push_back(ConfRepo(sectArg));
	  m_stringValues.clear();
	  m_stringListValues.clear();
	  m_booleanValues.clear();
	  m_uintValues.clear();
	  initValues();
	  initRepoValues();
	  initProvideValues();
	}
    }
  if (path[0] == "provide")
    {
      if (trim(sectArg).empty())
	throw NotImplementedException("Empty configuration file section argument");
      ConfProvideVector::size_type i = 0;
      while(i < m_root.provide.size() && m_root.provide[i].name != sectArg)
	i++;
      if (i >= m_root.provide.size())
	{
	  m_root.provide.push_back(ConfProvide(sectArg));
	  m_stringValues.clear();
	  m_stringListValues.clear();
	  m_booleanValues.clear();
	  m_uintValues.clear();
	  initValues();
	  initRepoValues();
	  initProvideValues();
	}
    }
  ConfigAdapter::onValue(path, sectArg, value, adding, pos);
}

void ConfigCenter::loadFromFile(const std::string& fileName)
{
  logMsg(LOG_DEBUG, "config:reading configuration from \'%s\'", fileName.c_str());
  assert(!fileName.empty());
  File f;
  f.openReadOnly(fileName);
  StringVector lines;
  f.readTextFile(lines);
  f.close();
  ConfigFile parser(*this, fileName);
  for(StringVector::size_type i = 0;i < lines.size();i++)
      parser.processLine(lines[i]);
}

void ConfigCenter::loadFromDir(const std::string& path)
{
  logMsg(LOG_DEBUG, "config:reading config file fragments from \'%s\'", path.c_str());
  StringVector files;
  std::auto_ptr<Directory::Iterator> it = Directory::enumerate(path);
  while(it->moveNext())
    {
      const std::string fileName = it->fullPath();
      if (File::isDir(fileName) || (!File::isRegFile(fileName) && !File::isSymLink(path)))//FIXME:Check exact symlink behaviour;
	continue;
      if (fileName.length() < 5 || fileName.substr(fileName.length() - 5) != ".conf")
	continue;
      files.push_back(fileName);
    }
  std::sort(files.begin(), files.end());
  for(StringVector::size_type i = 0;i < files.size();i++)
    loadFromFile(files[i]);
}

void ConfigCenter::addStringParam3(const std::string& path1,
				   const std::string& path2,
				   const std::string& path3,
				   std::string& value)
{
  assert(!path1.empty() && !path2.empty() && !path3.empty());
  StringValue stringValue(value);
  stringValue.canBeEmpty = 1;
  stringValue.path.push_back(path1);
  stringValue.path.push_back(path2);
  stringValue.path.push_back(path3);
  m_stringValues.push_back(stringValue);
}

void ConfigCenter::addNonEmptyStringParam3(const std::string& path1,
					   const std::string& path2,
					   const std::string& path3,
					   std::string& value)
{
  assert(!path1.empty() && !path2.empty() && !path3.empty());
  StringValue stringValue(value);
  stringValue.canBeEmpty = 0;
  stringValue.path.push_back(path1);
  stringValue.path.push_back(path2);
  stringValue.path.push_back(path3);
  m_stringValues.push_back(stringValue);
}

void ConfigCenter::addStringListParam3(const std::string& path1,
				   const std::string& path2,
				   const std::string& path3,
				   StringVector& value)
{
  assert(!path1.empty() && !path2.empty() && !path3.empty());
  StringListValue stringListValue(value);
  stringListValue.canContainEmptyItem = 1;
  stringListValue.canBeEmpty = 1;
  stringListValue.path.push_back(path1);
  stringListValue.path.push_back(path2);
  stringListValue.path.push_back(path3);
  m_stringListValues.push_back(stringListValue);
}

void ConfigCenter::addNonEmptyStringListParam3(const std::string& path1,
					   const std::string& path2,
					   const std::string& path3,
					   StringVector& value)
{
  assert(!path1.empty() && !path2.empty() && !path3.empty());
  StringListValue stringListValue(value);
  stringListValue.canContainEmptyItem = 0;
  stringListValue.canBeEmpty = 0;
  stringListValue.path.push_back(path1);
  stringListValue.path.push_back(path2);
  stringListValue.path.push_back(path3);
  m_stringListValues.push_back(stringListValue);
}

void ConfigCenter::addBooleanParam2(const std::string& path1,
				    const std::string& path2,
				    bool& value)
{
  assert(!path1.empty() && !path2.empty());
  BooleanValue boolValue(value);
  boolValue.path.push_back(path1);
  boolValue.path.push_back(path2);
  m_booleanValues.push_back(boolValue);
}


//Static functions;

/*
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
*/

bool checkUrl(const std::string& url)
{
  if (url.find("file:/") == 0)
    return url.length() > 6;
  if (url.find("://") == std::string::npos)
    return 0;
  return url.length() >= 4;
}

DEEPSOLVER_END_NAMESPACE

/*FIXME:
VarId SatBuilder::byProvidesPriorityList(const VarIdVector& vars, PkgId provideEntry) const
{

  assert(!vars.empty());
  const std::string provideName = m_scope.pkgIdToStr(provideEntry);
  TaskSolverProvideInfoVector::size_type k = 0;
  while(k < m_providesPriority.size() && provideName != m_providesPriority[k].name)
    k++;
  if (k >= m_providesPriority.size())
    return BadVarId;
  const StringVector& providers = m_providesPriority[k].providers;
  for(StringVector::size_type i = 0;i < providers.size();i++)
    {
      if (!m_scope.checkName(providers[i]))
	continue;
      const PackageId providerId = m_scope.strToPackageId(providers[i]);
      assert(providerId != BadPkgId);
      for(VarIdVector::size_type k = 0;k < vars.size();k++)
	if (providerId == m_scope.packageIdOfVarId(vars[k]))
	  return vars[k];
    }
  return BadVarId;
}

  for(ConfProvideVector::size_type i = 0;i < root.provide.size();i++)
    {
      assert(!trim(root.provide[i].name).empty());
      TaskSolverProvideInfo info(root.provide[i].name);
      for(StringVector::size_type k = 0;k < root.provide[i].providers.size();k++)
	info.providers.push_back(root.provide[i].providers[k]);
      taskSolverData.provides.push_back(info);
    }


*/
