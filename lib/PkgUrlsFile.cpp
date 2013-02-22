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

#include"deepsolver.h"
#include"PkgUrlsFile.h"

void PkgUrlsFile::open()
{
  assert(!m_conf.root().dir.pkgData.empty());
  const std::string fileName = Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_URLS_FILE_NAME);
  logMsg(LOG_DEBUG, "pkg-urls:opening file for package URLs saving with name \'%s\'", fileName.c_str());
  m_stream.open(fileName.c_str());
  if (!m_stream.is_open())
    {
      logMsg(LOG_ERR, "pkg-urls:error opening file \'%s\' for writing", fileName.c_str());
      throw OperationException(OperationException::InternalIOProblem); 
    }
}

void PkgUrlsFile::addPkg(const PkgFile& pkgFile, const std::string& url)
{
  m_stream << pkgFile.name << ":" << 
    pkgFile.epoch << ":" <<
    pkgFile.version << ":" <<
    pkgFile.release << ":" <<
    pkgFile.buildTime << ":" <<
    url << std::endl;
}

void PkgUrlsFile::close()
{
  assert(m_stream.is_open());
  m_stream.close();
}

void PkgUrlsFile::readUrls(const PkgVector& pkgs, StringVector& urls) const
{
  if (pkgs.empty())
    return;
  urls.resize(pkgs.size());
  StringVector ids;
  ids.resize(pkgs.size());
  for(PkgVector::size_type i = 0;i < pkgs.size();i++)
    {
      std::ostringstream ss;
      ss << pkgs[i].name << ":" << 
	pkgs[i].epoch << ":" <<
	pkgs[i].version << ":" <<
	pkgs[i].release << ":" <<
	pkgs[i].buildTime << ":";
      ids[i] = ss.str();
    }
  assert(!m_conf.root().dir.pkgData.empty());
  const std::string fileName = Directory::mixNameComponents(m_conf.root().dir.pkgData, PKG_URLS_FILE_NAME);
  std::ifstream is;
  logMsg(LOG_DEBUG, "pkg-urls:reading URLs for %zu packages from \'%s\'", pkgs.size(), fileName.c_str());
  is.open(fileName.c_str());
  if (!is.is_open())
    {
      logMsg(LOG_ERR, "pkg-urls:error opening file \'%s\' for reading", fileName.c_str());
      throw OperationException(OperationException::InternalIOProblem); 
    }
  while(1)
    {
      std::string line;
      std::getline(is, line);
      if (!is)
	break;
      StringVector::size_type i;
      for(i = 0;i < ids.size();i++)
	if (ids[i].length() + 1 <= line.length() && line.find(ids[i]) == 0)
	  break;
      if (i >= ids.size())
	continue;
      urls[i] = line.substr(ids[i].length());
    }
  is.close();
  logMsg(LOG_DEBUG, "pkg-urls:URLs reading completed");
}
