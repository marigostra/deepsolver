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

#ifndef DEEPSOLVER_CONFIG_DATA_H
#define DEEPSOLVER_CONFIG_DATA_H

namespace Deepsolver
{
  struct ConfDir
  {
    ConfDir()
      : pkgData(CONF_DEFAULT_PKG_DATA),
	pkgCache(CONF_DEFAULT_PKG_CACHE) {}

    std::string pkgData;
    std::string pkgCache;
  }; //struct ConfDir;

  struct ConfOs
  {
    StringVector transactReadAhead;
  }; //struct ConfOs;

  struct ConfRepo
  {
    ConfRepo()
      : enabled(1),
	takeDescr(1),
	takeFileList(0),
	takeSources(0) {}

    ConfRepo(const std::string& n)
      : name(n),
	enabled(1),
	takeDescr(1),
	takeFileList(0),
	takeSources(0) {}

    std::string name;
    bool enabled;
    std::string url;
    StringVector arch;
    StringVector components; 
    std::string vendor;
    bool takeDescr;
    bool takeFileList;
    bool takeSources;
  }; //struct ConfRepo;

  typedef std::vector<ConfRepo> ConfRepoVector;
  typedef std::list<ConfRepo> ConfRepoList;

  struct ConfProvide
  {
    ConfProvide() {}

    ConfProvide(const std::string& n)
      : name(n) {}

    std::string name;
    StringVector providers;
  }; //struct ConfProvide;

  typedef std::vector<ConfProvide> ConfProvideVector;
  typedef std::list<ConfProvide> ConfProvideList;

  struct ConfRoot
  {
    ConfRoot()
      : pkgListColumns(0),
	pkgListVersions(0),
	stopOnInvalidInstalledPkg(1),
	stopOnInvalidRepoPkg(1),
	tinyFileSizeLimit(104857) {} //FIXME:

    bool pkgListColumns;//FIXME:inaccessible;
    bool pkgListVersions;//FIXME:inaccessible;
    bool stopOnInvalidInstalledPkg;
    bool stopOnInvalidRepoPkg;
    //FIXME:Screen width;
    size_t tinyFileSizeLimit;//FIXME:Inaccessible;
    ConfDir dir;
    ConfOs os;
    ConfRepoVector repo;
    ConfProvideVector provide;
  }; //struct ConfRoot;
} //namespace Deepsolver;

#endif //DEEPSOLVER_CONFIG_DATA_H;
