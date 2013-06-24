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

#ifndef DEEPSOLVER_REPOSITORY_H
#define DEEPSOLVER_REPOSITORY_H

#include"deepsolver/ConfigCenter.h"
#include"deepsolver/AbstractPackageRecipient.h"
#include"deepsolver/RepoParams.h"
#include"deepsolver/PkgUrlsFile.h"

namespace Deepsolver
{
  class Repository
  {
  public:
    Repository(bool stopOnInvalidRepoPkg,
	       size_t tinyFileSizeLimit,
	       const ConfRepo& confRepo,
	       const std::string& arch,
	       const std::string& component)
      : m_stopOnInvalidRepoPkg(stopOnInvalidRepoPkg),
	m_tinyFileSizeLimit(tinyFileSizeLimit),
	m_name(confRepo.name),
	m_url(confRepo.url), 
	m_arch(arch),
	m_component(component),
	m_takeDescr(confRepo.takeDescr),
	m_takeFileList(confRepo.takeFileList),
	m_takeSources(confRepo.takeSources),
	m_compressionType(RepoParams::CompressionTypeNone),
	m_formatType(RepoParams::FormatTypeText)
    {
      assert(!m_url.empty());
      assert(!m_arch.empty());
      assert(!m_component.empty());
    }

    /**\brief The destructor*/
    virtual ~Repository() {}

  public:
    std::string getUrl() const
    {
      assert(!m_url.empty());
      return m_url;
    }

    void fetchInfoAndChecksum();
    void addIndexFilesForFetch(StringToStringMap& files);

    void loadPackageData(const StringToStringMap& files, 
			 AbstractPackageRecipient& transactData,
			 PkgUrlsFile& urlsFile,
			 AbstractPackageRecipient& pkgInfoData);

  private:
    std::string buildInfoFileUrl() const;
    std::string buildChecksumFileUrl() const;
    std::string buildBinaryPackageUrl(const PkgFile& pkgFile) const;

  private:
    bool m_stopOnInvalidRepoPkg;
    size_t m_tinyFileSizeLimit;
    std::string m_name, m_url, m_arch, m_component;
    bool m_takeDescr, m_takeFileList, m_takeSources;
    std::string m_checksumFileUrl, m_checksums;
    char m_compressionType;
    char m_formatType;
    std::string m_pkgFileUrl, m_pkgDescrFileUrl, m_pkgFileListFileUrl, m_srcFileUrl, m_srcDescrFileUrl;
    std::string m_checksumFileName;
  }; //class Repository;

  typedef std::vector<Repository> RepositoryVector;
  typedef std::list<Repository> RepositoryList;
} //namespace Deepsolver;

#endif //DEEPSOLVER_REPOSITORY_H;
