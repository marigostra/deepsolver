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

#ifndef DEEPSOLVER_CONFIG_H
#define DEEPSOLVER_CONFIG_H

//Configuration parameters;
#define DEFAULT_CONFIG_FILE_NAME "/etc/deepsolver/ds.conf"
#define DEFAULT_CONFIG_DIR_NAME "/etc/deepsolver/conf.d"
#define CONF_DEFAULT_PKG_DATA "/var/lib/deepsolver/pkg-data"
#define CONF_DEFAULT_PKG_CACHE "/var/lib/deepsolver/pkg-cache"
#define PKG_DATA_FILE_NAME "pkgs-data.bin"
#define PKG_URLS_FILE_NAME "pkgs-urls.txt"
#define PKG_DATA_FETCH_DIR "__tmp_pkg_data"

//Data files and directories;
#define COMPRESSION_SUFFIX_GZIP ".gz"

//Repo structure;
#define REPO_INDEX_DIR "base"
#define REPO_PACKAGES_DIR_PREFIX "RPMS."
#define REPO_SOURCES_DIR_PREFIX "SRPMS."

//Repo index structure;
#define REPO_INDEX_INFO_FILE "info"
#define REPO_INDEX_PACKAGES_FILE "rpms.data"
#define REPO_INDEX_PACKAGES_DESCR_FILE "rpms.descr.data"
#define REPO_INDEX_PACKAGES_FILELIST_FILE "rpms.filelist.data"
#define REPO_INDEX_SOURCES_FILE "srpms.data"
#define REPO_INDEX_SOURCES_DESCR_FILE "srpms.descr.data"
#define REPO_INDEX_PACKAGES_COMPLETE_FILE ".rpms.complete.data"
#define REPO_INDEX_MD5SUM_FILE "md5sum.txt"

//Info file;
#define INFO_FILE_FORMAT_TYPE "format_type"
#define INFO_FILE_FORMAT_TYPE_TEXT "text"
#define INFO_FILE_FORMAT_TYPE_BINARY "binary"
#define INFO_FILE_COMPRESSION_TYPE "compression_type"
#define INFO_FILE_COMPRESSION_TYPE_NONE "none"
#define INFO_FILE_COMPRESSION_TYPE_GZIP "gzip"
#define INFO_FILE_VERSION "version"
#define INFO_FILE_MD5SUM "md5sum_file"
#define INFO_FILE_FILTER_PROVIDES_BY_DIRS "filter_provides_by_dirs"
#define INFO_FILE_FILTER_PROVIDES_BY_REFS "filter_provides_by_refs"
#define INFO_FILE_EXCLUDE_REQUIRES "exclude_requires"
#define INFO_FILE_CHANGELOG_BINARY "changelog_binary"
#define INFO_FILE_CHANGELOG_SOURCES "changelog_sources"

#define DEEPSOLVER_IO_BLOCK_SIZE 2048

#endif //DEEPSOLVER_CONFIG_H;
