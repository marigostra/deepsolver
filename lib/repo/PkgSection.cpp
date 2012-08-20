/*
   Copyright 2011-2012 ALT Linux
   Copyright 2011-2012 Michael Pozhidaev

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

#include"deepsolver.h"
#include"PkgSection.h"

#define NAME_STR "n="
#define EPOCH_STR "e="
#define VERSION_STR "v="
#define RELEASE_STR "r="
#define ARCH_STR "arch="
#define URL_STR "URL="
#define LICENSE_STR "lic="
#define PACKAGER_STR "pckgr="
#define SUMMARY_STr "summ="
#define DESCRIPTION_STR "descr="
#define SRCRPM_STR "src="
#define BUILDTIME_STR "btime="
#define PROVIDES_STR "p:"
#define REQUIRES_STR "r:"
#define CONFLICTS_STR "c:"
#define OBSOLETES_STR "o:"
#define CHANGELOG_STR "cl:"

std::string PkgSection::saveBaseInfo(const PkgFile& pkgFile)
{
  std::ostringstream ss;
  ss << "[" << File::baseName(pkgFile.fileName) << "]" << std::endl;
  ss << NAME_STR << pkgFile.name << std::endl;
  ss << EPOCH_STR << pkgFile.epoch << std::endl;
  ss << VERSION_STR << pkgFile.version << std::endl;
  ss << RELEASE_STR << pkgFile.release << std::endl;
  ss << ARCH_STR << pkgFile.arch << std::endl;
  ss << BUILDTIME_STR  << pkgFile.buildTime << std::endl;
  for(NamedPkgRelVector::const_iterator it = pkgFile.provides.begin();it != pkgFile.provides.end();it++)
    {
      /*
       * The following operation must be done in both cases: in filtering by
       * references mode and without filtering at all. If there is no any filtering we just
       * saving all provides, if filtering is enabled we will proceed real
       * filtering on additional phase.
       */
      ss << PROVIDES_STR << saveNamedPkgRel(*it) << std::endl;
    }
  for(StringVector::size_type i = 0;i < pkgFile.fileList.size();i++)
    /*
     * If filtering by references is enabled we are writing all possible
     * provides to filter them on additional phase. If filterProvidesByDirs
     * string list is empty it means filtering by directories is disabled and
     * we also must write current file as provides. If filtering by
     * directories is enabled we are writing file as provides only if its
     * directory presents in directory list.
     */
    //FIXME:    if (m_filterProvidesByRefs || m_filterProvidesByDirs.empty() || fileFromDirs(*it, m_filterProvidesByDirs))
    ss << PROVIDES_STR << saveFileName(pkgFile.fileList[i]) << std::endl;
  for(NamedPkgRelVector::const_iterator it = pkgFile.requires.begin();it != pkgFile.requires.end();it++)
      //FIXME:      if (m_requireFilter.excludeRequire(it->pkgName))
    ss << REQUIRES_STR << saveNamedPkgRel(*it) << std::endl;
  for(NamedPkgRelVector::const_iterator it = pkgFile.conflicts.begin();it != pkgFile.conflicts.end();it++)
    ss << CONFLICTS_STR << saveNamedPkgRel(*it) << std::endl;
  for(NamedPkgRelVector::const_iterator it = pkgFile.obsoletes.begin();it != pkgFile.obsoletes.end();it++)
    ss << OBSOLETES_STR << saveNamedPkgRel(*it) << std::endl;
  ss << std::endl;
  return ss.str();
}

std::string PkgSection::saveDescr(const PkgFile& pkgFile, bool saveChangeLog)
{
  std::ostringstream ss;
  ss << "[" << File::baseName(pkgFile.fileName) << "]" << std::endl;
  ss << URL_STR << pkgFile.url << std::endl;
  ss << LICENSE_STR << pkgFile.license << std::endl;
  ss << PACKAGER_STR << pkgFile.packager << std::endl;
  ss << SUMMARY_STr << pkgFile.summary << std::endl;
  ss << DESCRIPTION_STR << encodeMultiline(pkgFile.description) << std::endl;
  ss << SRCRPM_STR << pkgFile.srcRpm << std::endl;
  if (saveChangeLog)
    {
      for(ChangeLog::size_type i = 0;i < pkgFile.changeLog.size();i++)
	ss << CHANGELOG_STR << encodeChangeLogEntry(pkgFile.changeLog[i]) << std::endl;
    }
  ss << std::endl;
  return ss.str();
}

std::string PkgSection::encodeMultiline(const std::string& s)
{
  std::string r;
  for(std::string::size_type i = 0;i < s.length();i++)
    {
      switch(s[i])
	{
	case '\\':
	  r += "\\\\";
	  break;
	case '\n':
	  r += "\\n";
	  break;
	case '\r':
	  continue;
	default:
	  r += s[i];
	}; //switch(s[i]);
    }
  return r;
}

std::string PkgSection::encodeChangeLogEntry(const ChangeLogEntry& entry)
{
  struct tm brTime;
  gmtime_r(&entry.time, &brTime);
  std::ostringstream s;
  s << (brTime.tm_year + 1900) << "-";
  if (brTime.tm_mon < 10)
    s << "0";
  s << (brTime.tm_mon + 1) << "-";
  if (brTime.tm_mday < 10)
    s << "0";
  s << brTime.tm_mday << std::endl;
  s << entry.name << std::endl;
  s << entry.text;
  return encodeMultiline(s.str());
}

std::string PkgSection::saveNamedPkgRel(const NamedPkgRel& r)
{
  std::string name;
  for(std::string::size_type i = 0;i < r.pkgName.length();i++)
    {
      if (r.pkgName[i] == ' ' || r.pkgName[i] == '\\')
	name += "\\";
      name += r.pkgName[i];
    }
  std::ostringstream s;
  s << name;
  if (r.ver.empty())
    return s.str();
  const bool less = r.type & VerLess, equals = r.type & VerEquals, greater = r.type & VerGreater;
  assert(!less || !greater);
  std::string t;
  if (less)
    t += "<";
  if (greater)
    t += ">";
  if (equals)
    t += "=";
  s << " " << t << " " << r.ver;
  return s.str();
}

std::string PkgSection::saveFileName(const std::string& fileName)
{
  std::string s;
  for(std::string::size_type i = 0;i < fileName.length();i++)
    {
      if (fileName[i] == ' ' || fileName[i] == '\\')
	s += "\\";
      s += fileName[i];
    }
  return s;
}

/*FIXME:
std::string PkgSection::getPkgRelName(const std::string& line)
{
  //Name is stored at the beginning of line until first space without previously used backslash;
  std::string res;
  for(std::string::size_type i = 0;i < line.length();i++)
    {
      if (line[i] == '\\')
	{
	  i++;
	  if (i < line.length())
	    res += line[i]; else 
	    return res + "\\";
	  continue;
	}
      if (line[i] == ' ')
	return res;
      res += line[i];
    } //for();
  return res;
}
*/
