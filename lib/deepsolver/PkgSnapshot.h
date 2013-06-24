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

#ifndef DEEPSOLVER_PKG_SNAPSHOT_H
#define DEEPSOLVER_PKG_SNAPSHOT_H

#include"deepsolver/AbstractPackageRecipient.h"

#define DEEPSOLVER_BEGIN_PKG_SNAPSHOT_NAMESPACE namespace PkgSnapshot {
#define DEEPSOLVER_END_PKG_SNAPSHOT_NAMESPACE }

namespace Deepsolver
{
  namespace PkgSnapshot
  {
    struct Relation
    {
      Relation()
	: pkgId(BadPkgId),
	  verDir(VerNone),
	  ver(NULL),
	  aux(0)   {}

      Relation(PkgId p)
	: pkgId(p),
	  verDir(VerNone),
	  ver(NULL),
	  aux(0) {}

      bool operator ==(const Relation& r) const
      {
	return pkgId == r.pkgId;
      }

      bool operator !=(const Relation& r) const
      {
	return pkgId != r.pkgId;
      }

      bool operator <(const Relation& r) const
      {
	return pkgId < r.pkgId;
      }

      bool operator >(const Relation& r) const
      {
	return pkgId > r.pkgId;
      }

      PkgId pkgId;
      VerDirection verDir;
      char* ver;
      size_t aux;//Purpose not defined but is used mostly as a storage for index of version string in string vectors;
    }; //struct Relation;

    typedef std::list<Relation> RelationList;
    typedef std::vector<Relation> RelationVector;

    struct Pkg
    {
      Pkg()
	: pkgId(BadPkgId),
	  epoch(0),
	  ver(NULL),
	  release(NULL),
	  buildTime(0),
	  requiresPos(0), requiresCount(0),
	  providesPos(0), providesCount(0),
	  conflictsPos(0), conflictsCount(0),
	  obsoletesPos(0), obsoletesCount(0), 
	  flags(0),
	  aux(0) {}

      Pkg(PkgId p)
	: pkgId(p),
	  epoch(0),
	  ver(NULL),
	  release(NULL),
	  buildTime(0),
	  requiresPos(0), requiresCount(0),
	  providesPos(0), providesCount(0),
	  conflictsPos(0), conflictsCount(0),
	  obsoletesPos(0), obsoletesCount(0), 
	  flags(0),
	  aux(0) {}

      bool operator ==(const Pkg& pkg) const
      {
	return pkgId == pkg.pkgId;
      }

      bool operator !=(const Pkg& pkg) const
      {
	return pkgId != pkg.pkgId;
      }

      bool operator <(const Pkg& pkg) const
      {
	return pkgId < pkg.pkgId;
      }

      bool operator >(const Pkg& pkg) const
      {
	return pkgId > pkg.pkgId;
      }

      PkgId pkgId;
      Epoch epoch;
      char* ver;
      char* release;
      time_t buildTime;
      size_t requiresPos, requiresCount;
      size_t providesPos, providesCount;
      size_t conflictsPos, conflictsCount;
      size_t obsoletesPos, obsoletesCount;
      int flags;
      size_t aux;//Purpose not defined but is used mostly as a storage for index of version string in string vectors;
    }; //struct Pkg;

    typedef std::list<Pkg> PkgList;
    typedef std::vector<Pkg> PkgVector;

    struct Snapshot
    {
      StringVector pkgNames;
      PkgVector pkgs;
      RelationVector relations;
    }; //struct Snapshot; 

    void addNewPkg(Snapshot& snapshot,
		   const PkgFile& pkgFile,
		   ConstCharVector& strings,
		   StringToPkgIdMap& stringToPkgIdMap);

    bool locateRange(const Snapshot& snapshot,
		     PkgId pkgId,
		     VarId& fromPos,
		     VarId& toPos );

    void enhance(Snapshot& snapshot,
		 const Deepsolver::PkgVector& enhanceWith,
		 int flags,
		 ConstCharVector& strings);

    void rearrangeNames(Snapshot& snapshot);
    bool checkName(const Snapshot& snapshot, const std::string& name);
    PkgId strToPkgId(const Snapshot& snapshot, const std::string& name);
    std::string pkgIdToStr(const Snapshot& snapshot, PkgId pkgId);

    void loadFromFile(Snapshot& snapshot,
		      const std::string& fileName,
		      ConstCharVector& strings);

    void saveToFile(const Snapshot& snapshot,
		    const std::string& fileName,
		    ConstCharVector& strings);

    size_t getScore(const Snapshot& snapshot);
    bool theSameVersion(const Deepsolver::Pkg& p1, const Deepsolver::PkgSnapshot::Pkg& p2);

    class PkgRecipientAdapter: public AbstractPackageRecipient
    {
    public:
      /**\brief The constructor*/
      PkgRecipientAdapter(Snapshot& snapshot,
			  ConstCharVector& strings,
			  StringToPkgIdMap& stringToPkgIdMap)
	: m_snapshot(snapshot),
	  m_strings(strings),
	  m_stringToPkgIdMap(stringToPkgIdMap) {}

      /**\brief The destructor*/
      virtual ~PkgRecipientAdapter() {}

    public:
      void onNewPkgFile(const PkgFile& pkgFile)
      {
	addNewPkg(m_snapshot, pkgFile, m_strings, m_stringToPkgIdMap);
      }

    private:
      Snapshot& m_snapshot;
      ConstCharVector& m_strings;
      StringToPkgIdMap m_stringToPkgIdMap;
    }; //class PkgRecipientAdapter;
  } //namespace PkgSnapshot;
} //namespace Deepsolver;

#endif //DEEPSOLVER_PKG_SNAPSHOT_H;
