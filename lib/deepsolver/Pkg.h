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

#ifndef DEEPSOLVER_PKG_H
#define DEEPSOLVER_PKG_H

namespace Deepsolver
{
  class VerSubset
  {
  public:
    VerSubset()
      : type(VerNone) {}

    VerSubset(const std::string& v)
      : version(v),
	type(VerEquals) {}

    VerSubset(const std::string& v, VerDirection t)
      : version(v),
	type(t) {}

    VerSubset(const std::string& v,
		bool l,
		bool e,
		bool g)
      : version(v),
	type(0)
    {
      assert(!l || !g);
      if (l)
	type |= VerLess;
      if (e)
	type |= VerEquals;
      if (g)
	type |= VerGreater;
    }

  public:
    bool isLess() const
    {
      return type & VerLess;
    }

    bool isEqual() const
    {
      return type & VerEquals;
    }

    bool isGreater() const
    {
      return type & VerGreater;
    }

  public:
    std::string version;
    VerDirection type;
  }; //class VerSubset;

  typedef std::list<VerSubset> VerSubsetList;
  typedef std::vector<VerSubset> VerSubsetVector;

  /**\brief The relation between two packages with the package reference by name
   *
   * This class contains information about a relation between two
   * packages. Relation data includes version restriction with version subset
   * direction using values from the list "less", "less or equals", "equals",
   * "greater or equals", "greater". The package reference 
   * with this class is saved through its name (there can 
   * also be a reference through index in some table).
   *
   * \sa IdPkgRel
   */
  class NamedPkgRel
  {
  public:
    /**\brief The default constructor*/
    NamedPkgRel()
      : type(0) {}

    /**\brief The constructor with package name
     *
     * \param [in] pName The name of the package to create reference to
     */
    NamedPkgRel(const std::string& pName)
      : pkgName(pName),
	type(0) {}

    /**\brief The constructor with full relation specification
     *
     * \param [in] pName The name of the package to create reference to
     * \param [in] t The version subset direction
     * \param [in] v The version restriction value
     */
    NamedPkgRel(const std::string& pName,
		VerDirection t,
		const std::string& v)
      : pkgName(pName),
	type(t),
	ver(v) {}

  public:
    /**\brief Checks the relation is consistent
     * \return Non-zero if there are no consistency breaks or zero otherwise
     */
    bool valid() const
    {
      if (type > VerLess + VerGreater + VerEquals)
	return 0;
      if ((type & VerLess) && (type & VerGreater))
	return 0;
      if (pkgName.empty())
	return 0;
      if (ver.empty() && type != VerNone)
	return 0;
      if (!ver.empty() && type == VerNone)
	return 0;
      return 1;
    }

    /**\brief Checks if there is version restriction
     * \return Non-zero if the version is restricted with this relation
     */
    bool verRestricted() const
    {
      assert(ver.empty() || type != VerNone);
      assert(!ver.empty() || type == VerNone);
      return type != VerNone;
    }

  public:
    std::string pkgName;
    VerDirection type;
    std::string ver;
  }; //class NamedPkgRel;

  typedef std::list<NamedPkgRel> NamedPkgRelList;
  typedef std::vector<NamedPkgRel> NamedPkgRelVector;

  class IdPkgRel
  {
  public:
    IdPkgRel()
      : pkgId(BadPkgId), verDir(VerNone) {}

    explicit IdPkgRel(PackageId id)
      : pkgId(id), verDir(VerNone) {}

    IdPkgRel(PackageId id, const std::string& v)
      : pkgId(id), verDir(VerEquals), ver(v) {}

    IdPkgRel(PackageId id, VerDirection dir, const std::string& v)
      : pkgId(id), verDir(dir), ver(v) {}

    IdPkgRel(PackageId id, const VerSubset& cond)
      : pkgId(id), verDir(cond.type), ver(cond.version) {}

  public:
    bool operator ==(const IdPkgRel& rel) const
    {
      return pkgId == rel.pkgId && verDir == rel.verDir && ver == rel.ver;
    }

    bool operator !=(const IdPkgRel& rel) const
    {
      return pkgId != rel.pkgId || verDir != rel.verDir || ver == rel.ver;
    }

  public:
    /**\brief Checks if there is version restriction
     * \return Non-zero if the version is restricted with this relation
     */
    bool verRestricted() const
    {
      assert(ver.empty() || verDir != VerNone);
      assert(!ver.empty() || verDir == VerNone);
      return verDir != VerNone;
    }

    std::string verString() const
    {
      if (!verRestricted())
	return "";
      std::string s;
      if (verDir & VerLess)
	s += "<";
      if (verDir & VerGreater)
	s += ">";
      if (verDir & VerEquals)
	s += "=";
      s += " " + ver;
      return s;
    }

    VerSubset extractVerSubset() const
    {
      return VerSubset(ver, verDir);
    }

    bool valid() const
    {
      if (pkgId == BadPkgId)
	return 0;
      if (ver.empty() && verDir != VerNone)
	return 0;
      if (!ver.empty() && verDir == VerNone)
	return 0;
      return 1;
    }

  public:
    PkgId pkgId;
    VerDirection verDir;
    std::string ver;
  }; //class IdPkgRel;

  typedef std::vector<IdPkgRel> IdPkgRelVector;
  typedef std::list<IdPkgRel> IdPkgRelList;

  class ChangeLogEntry
  {
  public:
    ChangeLogEntry()
      : time(0) {}

    ChangeLogEntry(time_t tm, const std::string& nm, const std::string& txt)
      : time(tm), name(nm), text(txt) {}

  public:
    time_t time;
    std::string name;
    std::string text;
  }; //class ChangeLogEntry;

  typedef std::vector<ChangeLogEntry> ChangeLogEntryVector;
  typedef std::list<ChangeLogEntry> ChangeLogEntryList;
  typedef ChangeLogEntryVector ChangeLog;

  class PkgBase
  {
  public:
    PkgBase()
      : epoch(0), buildTime(0)  {}
    virtual ~PkgBase() {}

  public:
    virtual bool valid() const
    {
      if (name.empty() || version.empty() || release.empty())
	return 0;
      return 1;
    }

    bool theSameAs(const PkgBase& p) const
    {
      return (name == p.name &&
	      version == p.version &&
	      release == p.release &&
	      buildTime == p.buildTime);
    }

  public:
    Epoch epoch;
    std::string name, version, release;
    std::string arch;
    std::string summary, description;
    std::string packager, group, url, license;
    std::string srcRpm;
    time_t buildTime;
  }; //class PkgBase;

  class PkgFileBase: public PkgBase
  {
  public:
    PkgFileBase()
      : isSource(0)  {}

    virtual ~PkgFileBase() {}

  public:
    std::string fileName;
    bool isSource;
  }; //class PkgFileBase;

  class PkgRelations
  {
  public:
    NamedPkgRelVector requires, provides, conflicts, obsoletes;
    StringVector fileList;
  }; //class PkgRelations;

  class Pkg: public PkgBase, public PkgRelations
  {
  public:
    /**\brief The default constructor*/
    Pkg() {}

    /**\brief The destructor*/
    virtual ~Pkg() {}

  public:
    virtual bool valid() const
    {
      if (!PkgBase::valid())
	return 0;
      for(NamedPkgRelVector::size_type i = 0;i < requires.size();i++)
	if (!requires[i].valid())
	  return 0;
      for(NamedPkgRelVector::size_type i = 0;i < conflicts.size();i++)
	if (!conflicts[i].valid())
	  return 0;
      for(NamedPkgRelVector::size_type i = 0;i < provides.size();i++)
	if (!provides[i].valid())
	  return 0;
      for(NamedPkgRelVector::size_type i = 0;i < obsoletes.size();i++)
	if (!obsoletes[i].valid())
	  return 0;
      for(StringVector::size_type i = 0;i < fileList.size();i++)
	if (fileList[i].empty())
	  return 0;
      return 1;
    }

  public:
    ChangeLog changeLog;
  };

  typedef std::vector<Pkg> PkgVector;
  typedef std::list<Pkg> PkgList;

  class PkgFile: public PkgFileBase, public PkgRelations 
  {
  public:
    /**\brief The default constructor*/
    PkgFile() {}

    /**\brief The destructor*/
    virtual ~PkgFile() {}

  public:
    virtual bool valid() const
    {
      if (!PkgBase::valid())
	return 0;
      for(NamedPkgRelVector::size_type i = 0;i < requires.size();i++)
	if (!requires[i].valid())
	  return 0;
      for(NamedPkgRelVector::size_type i = 0;i < conflicts.size();i++)
	if (!conflicts[i].valid())
	  return 0;
      for(NamedPkgRelVector::size_type i = 0;i < provides.size();i++)
	if (!provides[i].valid())
	  return 0;
      for(NamedPkgRelVector::size_type i = 0;i < obsoletes.size();i++)
	if (!obsoletes[i].valid())
	  return 0;
      for(StringVector::size_type i = 0;i < fileList.size();i++)
	if (fileList[i].empty())
	  return 0;
      return 1;
    }

    public:
    ChangeLog changeLog;
  };

  typedef std::vector<PkgFile> PkgFileVector;
  typedef std::list<PkgFile> PkgFileList;
} //namespace Deepsolver;

#endif //DEEPSOLVER_PKG_H;
