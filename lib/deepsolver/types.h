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

#ifndef DEEPSOLVER_TYPES_H
#define DEEPSOLVER_TYPES_H

namespace Deepsolver
{
  /*
   * PackageId type is the identifier of an item from the set built by
   * included strings from set of package names and set of all their
   * provides without any additional information such as version or anything
   * else.  Set of packages to consider consists of three parts: packages
   * from accessible repositories, installed packages in the system and
   * packages provided by user as files in the file system or by URL (may
   * absent in accessible repositories). 
   *
   * This type is basically  used due to performance resons.
   */
  typedef size_t PackageId;//Obsolete;
  typedef size_t PkgId;
  enum {BadPkgId = (PkgId)-1};

  typedef std::vector<PackageId> PackageIdVector;
  typedef std::list<PackageId> PackageIdList;
  typedef std::map<std::string, PkgId> StringToPkgIdMap;

  /*
   * The VarId type is used to identify one exact package from the set of
   * all known packages. The set of all known packages consists of all
   * packages from the accessible repositories, all installed packages and
   * packages provided by user as files in file system or by URL. Two or
   * more packages can be identify by the same VarId if they have the same
   * name, the same epoch, the same version, the same release and the same
   * build time. In all other cases packages must be considered as
   * different packages and must be identified by different values of
   * VarId. In this meaning values of VarId can be used as variables in SAT
   * formulas.
   *
   * The VarId must not be confused with PackageId type.
   */
  typedef size_t VarId;
  enum {BadVarId = (VarId)-1};
  typedef std::vector<VarId> VarIdVector;
  typedef std::list<VarId> VarIdList;
  typedef std::set<VarId> VarIdSet;
  typedef std::map<VarId, VarId> VarIdToVarIdMap;

  typedef char VerDirection;
  typedef unsigned short Epoch;

  enum {
    VerNone = 0,
    VerLess = 1,
    VerEquals = 2,
    VerGreater = 4
  };

  enum {
    PkgFlagInstalled = 1
  };

  class UserTaskItemToInstall
  {
  public:
    UserTaskItemToInstall()
      : verDir(VerNone) {}

    UserTaskItemToInstall(const std::string& n)
      : pkgName(n), verDir(VerNone) {}

    UserTaskItemToInstall(const std::string& n, VerDirection d, const std::string& v)
      : pkgName(n), verDir(d), version(v) {}

  public:
    std::string toString() const
    {
      if (verDir == VerNone)
	return pkgName;
      std::string s = pkgName;
      s += " ";
      if (verDir & VerLess)
	s += "<";
      if (verDir & VerGreater)
	s += ">";
      if (verDir & VerEquals)
	s += "=";
      return s + " " + version;
    }

  public:
    std::string pkgName;
    VerDirection verDir;
    std::string version;
  }; //class UserTaskItemToINstall;

  typedef std::vector<UserTaskItemToInstall> UserTaskItemToInstallVector;
  typedef std::list<UserTaskItemToInstall> UserTaskItemToInstallList;

  class UserTask 
  {
  public:
    UserTask() {}
    ~UserTask() {}

  public:
    UserTaskItemToInstallVector itemsToInstall;
    StringSet urlsToInstall;
    StringSet namesToRemove;
  }; //class UserTask;

  struct DateTime
  {
    unsigned short year;
    unsigned char month, day, hour, minute, second;
  }; //struct DateTime;

} ///namespace Deepsolver;

#endif //DEEPSOLVER_TYPES_H;
