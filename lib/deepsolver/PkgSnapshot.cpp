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

#include"deepsolver/deepsolver.h"
#include"deepsolver/PkgSnapshot.h"
#include"deepsolver/InlineIO.h"

//#define THROW_INTERNAL_ERROR throw Deepsolver::OperationException(Deepsolver::OperationException::InternalIOProblem)
#define THROW_INTERNAL_ERROR //FIXME:

DEEPSOLVER_BEGIN_NAMESPACE
DEEPSOLVER_BEGIN_PKG_SNAPSHOT_NAMESPACE

// For general operations;

static void processRelations(Snapshot& snapshot,
			     const NamedPkgRelVector& rels,
			     size_t& pos,
			     size_t& count,
			     ConstCharVector& strings,
			     StringToPkgIdMap& stringToPkgIdMap);

static PkgId registerName(Snapshot& snapshot,
			  const std::string& name,
			  StringToPkgIdMap& stringToPkgIdMap);

//For enhancing;

static void addRelationsForEnhancing(Snapshot& snapshot,
				     const NamedPkgRelVector& relations,
				     size_t& pos,
				     size_t& count,
				     char* stringBuf, 
				     size_t& stringBufOffset);

static void addProvidesForEnhancing(Snapshot& snapshot,
				    const NamedPkgRelVector& relations,
				    const StringVector& fileList,
				    size_t& pos,
				    size_t& count,
				    char* stringBuf,
				    size_t& stringBufOffset);

static char* putStringInBuffer(Snapshot& snapshot,
			       char* buf,
			       size_t& offset,
			       const std::string& value);

//For loading;

static void readNames(Snapshot& snapshot,
		      std::ifstream& s,
		      size_t namesBufSize);

static void onNewName(Snapshot& snapshot,
		      const char* name,
		      bool complete,
		      std::string& chunk);

// For printing;

static void printRelations(const Snapshot& snapshot,
			   const std::string& title,
			   std::ostream& s,
			   RelationVector::size_type pos,
			   RelationVector::size_type count);

void addNewPkg(Snapshot& snapshot,
	       const PkgFile& pkgFile,
	       ConstCharVector& strings,
	       StringToPkgIdMap& stringToPkgIdMap)
{
  assert(pkgFile.valid());
  //We are interested only in name, epoch, version, release, and all relations;
  Deepsolver::PkgSnapshot::Pkg pkg;
  pkg.pkgId = registerName(snapshot, pkgFile.name, stringToPkgIdMap);
  pkg.epoch = pkgFile.epoch;
  pkg.ver = new char[pkgFile.version.length() + 1];
  strcpy(pkg.ver, pkgFile.version.c_str());
  strings.push_back(pkg.ver);
  pkg.aux = strings.size() - 1;
  pkg.release = new char[pkgFile.release.length() + 1];
  strcpy(pkg.release, pkgFile.release.c_str());
  strings.push_back(pkg.release);
  //No need to save index of release string in strings vector since it always has the value of version string index +1;
  pkg.buildTime = pkgFile.buildTime;
  processRelations(snapshot, pkgFile.requires, pkg.requiresPos, pkg.requiresCount, strings, stringToPkgIdMap);
  processRelations(snapshot, pkgFile.conflicts, pkg.conflictsPos, pkg.conflictsCount, strings, stringToPkgIdMap);
  processRelations(snapshot, pkgFile.provides, pkg.providesPos, pkg.providesCount, strings, stringToPkgIdMap);
  processRelations(snapshot, pkgFile.obsoletes, pkg.obsoletesPos, pkg.obsoletesCount, strings, stringToPkgIdMap);
  snapshot.pkgs.push_back(pkg);
}

void processRelations(Snapshot& snapshot,
		 const NamedPkgRelVector& rels,
		 size_t& pos,
		 size_t& count,
		 ConstCharVector& strings,
		 StringToPkgIdMap& stringToPkgIdMap)
{
  if (rels.empty())
    {
      pos = 0;
      count = 0;
      return;
    }
  pos = snapshot.relations.size();
  count = rels.size();
  for(NamedPkgRelVector::size_type i = 0;i < count;i++)
    {
      const NamedPkgRel& rel = rels[i];
      assert(rel.valid());
      Relation newEntry;
      newEntry.pkgId = registerName(snapshot, rel.pkgName, stringToPkgIdMap);
      if (!rel.ver.empty())
	{
	  assert(rel.type != 0);
	  newEntry.verDir = rel.type;
	  newEntry.ver = new char[rel.ver.length() + 1];
	  strcpy(newEntry.ver, rel.ver.c_str());
	  strings.push_back(newEntry.ver);
	  newEntry.aux = strings.size() - 1;
	} else
	{
	  newEntry.verDir = VerNone;
	  newEntry.ver = NULL;
	  newEntry.aux = (size_t)-1;
	}
      snapshot.relations.push_back(newEntry);
    }
}

PkgId registerName(Snapshot& snapshot,
		   const std::string& name,
		   StringToPkgIdMap& stringToPkgIdMap)
{
  assert(!name.empty());
  StringToPkgIdMap::const_iterator it = stringToPkgIdMap.find(name);
  if (it != stringToPkgIdMap.end())
    return it->second;
  snapshot.pkgNames.push_back(name);
  const PkgId pkgId = snapshot.pkgNames.size() - 1;
  stringToPkgIdMap.insert(StringToPkgIdMap::value_type(name, pkgId));
  return pkgId;
}

bool locateRange(const Snapshot& snapshot,
		 PkgId pkgId,
		 VarId& fromPos,
		 VarId& toPos )
{
  Deepsolver::PkgSnapshot::Pkg pkg(pkgId);
  return Dichotomy<Deepsolver::PkgSnapshot::Pkg>().findMultiple(snapshot.pkgs, pkg, fromPos, toPos);
}

void enhance(Snapshot& snapshot,
	     const Deepsolver::PkgVector& enhanceWith,
	     int flags,
	     ConstCharVector& strings)
{
  logMsg(LOG_DEBUG, "snapshot:starting enhancing procedure with %zu new packages", enhanceWith.size());
  const clock_t started = clock();
  if (enhanceWith.empty())
    return;
  //First of all we should collect new package names;
  StringSet newNames;
  for(Deepsolver::PkgVector::size_type i = 0;i < enhanceWith.size();i++)
    {
      const Deepsolver::Pkg& pkg = enhanceWith[i];
      if (!checkName(snapshot, pkg.name))
	newNames.insert(pkg.name);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.requires.size();k++)
	if (!checkName(snapshot, pkg.requires[k].pkgName))
	  newNames.insert(pkg.requires[k].pkgName);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.provides.size();k++)
	if (!checkName(snapshot, pkg.provides[k].pkgName))
	  newNames.insert(pkg.provides[k].pkgName);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.obsoletes.size();k++)
	if (!checkName(snapshot, pkg.obsoletes[k].pkgName))
	  newNames.insert(pkg.obsoletes[k].pkgName);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.conflicts.size();k++)
	if (!checkName(snapshot , pkg.conflicts[k].pkgName))
	  newNames.insert(pkg.conflicts[k].pkgName);
    } //for(enhanceWith);
  logMsg(LOG_DEBUG, "snapshot:%zu new package names are gathered", newNames.size());
  //OK, we have new package names and now can add them with rearranging;
  for(StringSet::const_iterator it = newNames.begin();it != newNames.end();it++)
    snapshot.pkgNames.push_back(*it);
  rearrangeNames(snapshot);
  //What is a size of buffer we need for version strings?
  size_t versionStringBufSize = 0;
  for(Deepsolver::PkgVector::size_type i = 0;i < enhanceWith.size();i++)
    {
      const Deepsolver::Pkg& pkg = enhanceWith[i];
      assert(!pkg.version.empty());
      assert(!pkg.release.empty());
      versionStringBufSize += pkg.version.length() + 1;
      versionStringBufSize += pkg.release.length() + 1;
      for(NamedPkgRelVector::size_type k = 0;k < pkg.requires.size();k++)
	versionStringBufSize += (pkg.requires[k].type == VerNone?0:pkg.requires[k].ver.length() + 1);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.provides.size();k++)
	versionStringBufSize += (pkg.provides[k].type == VerNone?0:pkg.provides[k].ver.length() + 1);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.conflicts.size();k++)
	versionStringBufSize += (pkg.conflicts[k].type == VerNone?0:pkg.conflicts[k].ver.length() + 1);
      for(NamedPkgRelVector::size_type k = 0;k < pkg.obsoletes.size();k++)
	versionStringBufSize += (pkg.obsoletes[k].type == VerNone?0:pkg.obsoletes[k].ver.length() + 1);
    }
  logMsg(LOG_DEBUG, "snapshot:string buffer for new versions and releases must have length of %zu bytes", versionStringBufSize);
  assert(versionStringBufSize > 0);
  strings.push_back(new char[versionStringBufSize]);
  char* versionStringBuf = (char*)strings.back();
  size_t offset = 0;
  //We got all strings prepared; adding new entries;
  for(Deepsolver::PkgVector::size_type i = 0;i < enhanceWith.size();i++)
    {
      const Deepsolver::Pkg& pkg = enhanceWith[i];
      PkgSnapshot::Pkg newEntry;
      assert(checkName(snapshot, pkg.name));
      newEntry.pkgId = strToPkgId(snapshot, pkg.name);
      newEntry.epoch = pkg.epoch;
      newEntry.ver = putStringInBuffer(snapshot, versionStringBuf, offset, pkg.version);
      newEntry.release = putStringInBuffer(snapshot, versionStringBuf, offset, pkg.release);
      newEntry.buildTime = pkg.buildTime;
      newEntry.flags = flags;
      addRelationsForEnhancing(snapshot, pkg.requires, newEntry.requiresPos, newEntry.requiresCount, versionStringBuf, offset);
      addProvidesForEnhancing(snapshot, pkg.provides, pkg.fileList, newEntry.providesPos, newEntry.providesCount, versionStringBuf, offset);
      addRelationsForEnhancing(snapshot, pkg.conflicts, newEntry.conflictsPos, newEntry.conflictsCount, versionStringBuf, offset);
      addRelationsForEnhancing(snapshot, pkg.obsoletes, newEntry.obsoletesPos, newEntry.obsoletesCount, versionStringBuf, offset);
      snapshot.pkgs.push_back(newEntry);
    }

  logMsg(LOG_DEBUG, "%zu %zu", offset, versionStringBufSize);

  assert(offset == versionStringBufSize);
  std::sort(snapshot.pkgs.begin(), snapshot.pkgs.end());
  const double duration = ((double)clock() - started) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "snapshot:enhancing is completed in %f sec", duration);
}

void addRelationsForEnhancing(Snapshot& snapshot,
			      const NamedPkgRelVector& relations,
			      size_t& pos,
			      size_t& count,
			      char* stringBuf, 
			      size_t& stringBufOffset)
{
  assert(stringBuf);
  if (relations.empty())
    {
      pos = 0;
      count = 0;
      return;
    }
  pos = snapshot.relations.size();
  count = relations.size();
  for(NamedPkgRelVector::size_type i = 0;i < count;i++)
    {
      const NamedPkgRel& relation = relations[i];
      Deepsolver::PkgSnapshot::Relation newEntry;
      assert(!relation.pkgName.empty());
      assert(checkName(snapshot, relation.pkgName));
      newEntry.pkgId = strToPkgId(snapshot, relation.pkgName);
      if (relation.type != VerNone)
	{
	  assert(!relation.ver.empty());
	  newEntry.verDir = relation.type;
	  newEntry.ver = putStringInBuffer(snapshot, stringBuf, stringBufOffset, relation.ver);
	} else
	{
	  assert(relation.ver.empty());
	  newEntry.verDir = VerNone;
	  newEntry.ver = NULL;
	}
      snapshot.relations.push_back(newEntry);
    }
}

void addProvidesForEnhancing(Snapshot& snapshot,
			     const NamedPkgRelVector& relations,
			     const StringVector& fileList,
			     size_t& pos,
			     size_t& count,
			     char* stringBuf,
			     size_t& stringBufOffset)
{
  assert(stringBuf != NULL);
  pos = snapshot.relations.size();
  count = 0;
  for(NamedPkgRelVector::size_type i = 0;i < relations.size();i++)
    {
      const NamedPkgRel& relation = relations[i];
      Deepsolver::PkgSnapshot::Relation newEntry;
      assert(!relation.pkgName.empty());
      assert(checkName(snapshot, relation.pkgName));
      newEntry.pkgId = strToPkgId(snapshot, relation.pkgName);
      if (relation.type != VerNone)
	{
	  assert(!relation.ver.empty());
	  newEntry.verDir = relation.type;
	  newEntry.ver = putStringInBuffer(snapshot, stringBuf, stringBufOffset, relation.ver);
	} else
	{
	  assert(relation.ver.empty());
	  newEntry.verDir = VerNone;
	  newEntry.ver = NULL;
	}
      snapshot.relations.push_back(newEntry);
      count++;
    }
  for(StringVector::size_type i = 0;i < fileList.size();i++)
    {
      const std::string& value = fileList[i];
      /*
       * Quite tricky place: we are registering as provides only file names
       * already known in snapshot. If an enhancing is performed only once this
       * idea is absolutely safe, but if there will be more enhance procedures
       * (that actually never happens now) we can expect problems leading to package
       * unmets during solver work.
       */
      if (value.empty() || !checkName(snapshot, value))
	continue;
      Deepsolver::PkgSnapshot::Relation newEntry;
      newEntry.pkgId = strToPkgId(snapshot, value);
      newEntry.verDir = VerNone;
      newEntry.ver = NULL;
      snapshot.relations.push_back(newEntry);
      count++;
    }
  if (count == 0)
    pos = 0;
}

char* putStringInBuffer(Snapshot& snapshot,
			  char* buf,
			  size_t& offset,
			  const std::string& value)
{
  assert(buf);
  char* origPlace = &buf[offset];
  strcpy(&buf[offset], value.c_str());
  offset += (value.length() + 1);
  return origPlace;
}

bool checkName(const Snapshot& snapshot, const std::string& name)
{
  assert(!name.empty());
  if (snapshot.pkgNames.empty())
    return 0;
  StringVector::size_type pos;
  return Dichotomy<std::string>().findSingle(snapshot.pkgNames, name, pos);
}

PkgId strToPkgId(const Snapshot& snapshot, const std::string& name)
{
  assert(!name.empty());
  assert(!snapshot.pkgNames.empty());
  PkgId pkgId = BadPkgId;
  if (!Dichotomy<std::string>().findSingle(snapshot.pkgNames, name, pkgId))
    pkgId = BadPkgId;
  return pkgId;
}

std::string pkgIdToStr(const Snapshot& snapshot, PkgId pkgId)
{
  assert(pkgId != BadPkgId && pkgId < snapshot.pkgNames.size());
  return snapshot.pkgNames[pkgId];
}

void rearrangeNames(Snapshot& snapshot)
{
  const clock_t start = clock();
  if (snapshot.pkgNames.size() < 2)
    return;
  StringVector newNames(snapshot.pkgNames);
  std::sort(newNames.begin(), newNames.end());
  SizeVector newPositions;
  newPositions.resize(snapshot.pkgNames.size());
  assert(snapshot.pkgNames.size() == newNames.size() && snapshot.pkgNames.size() == newPositions.size());
      Dichotomy<std::string> d;
  for(StringVector::size_type i = 0;i < snapshot.pkgNames.size();i++)
    {
      StringVector::size_type res = 0;
      const bool found = d.findSingle(newNames, snapshot.pkgNames[i], res);
      assert(found);
      assert(res < newNames.size());
      assert(snapshot.pkgNames[i] == newNames[res]);
      newPositions[i] = res;
    }
  for(Deepsolver::PkgSnapshot::PkgVector::size_type i = 0;i < snapshot.pkgs.size();i++)
    {
      Deepsolver::PkgSnapshot::Pkg& pkg = snapshot.pkgs[i];
      assert(pkg.pkgId < newPositions.size());
      pkg.pkgId = newPositions[pkg.pkgId];
    }
  for(Deepsolver::PkgSnapshot::RelationVector::size_type i = 0;i < snapshot.relations.size();i++)
    {
      Deepsolver::PkgSnapshot::Relation& r = snapshot.relations[i];
      assert(r.pkgId < newPositions.size());
      r.pkgId = newPositions[r.pkgId];
    }
  snapshot.pkgNames = newNames;
  std::sort(snapshot.pkgs.begin(), snapshot.pkgs.end());
  const double duration = ((double)clock() - start) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "snapshot:names rearranging is done in %f sec", duration);
}

void loadFromFile(Snapshot& snapshot,
		  const std::string& fileName,
		  ConstCharVector& strings)
{
  assert(!fileName.empty());
  logMsg(LOG_DEBUG, "snapshot:starting reading from binary file \'%s\'", fileName.c_str());
  assert(snapshot.pkgNames.empty());
  assert(snapshot.pkgs.empty());
  assert(snapshot.relations.empty());
  std::ifstream s(fileName.c_str());
  if (!s.is_open())
    {
      logMsg(LOG_ERR, "An error is occurred while opening \'%s\' for packages snapshto reading", fileName.c_str());
      THROW_INTERNAL_ERROR;//FIXME:More information;
    }
  //Reading numbers of records;
  const size_t stringBufSize = readSizeValue(s);
  logMsg(LOG_DEBUG, "snapshot:%zu bytes in all string constants with trailing zeroes", stringBufSize);
  const size_t nameCount = readSizeValue(s);
  logMsg(LOG_DEBUG, "snapshot:%zu package names", nameCount);
  const size_t namesBufSize = readSizeValue(s);
  logMsg(LOG_DEBUG, "snapshot:%zu bytes in all package names with trailing zeroes", namesBufSize);
  snapshot.pkgs.resize(readSizeValue(s));
  logMsg(LOG_DEBUG, "snapshot:%zu packages", snapshot.pkgs.size());
  snapshot.relations.resize(readSizeValue(s));
  logMsg(LOG_DEBUG, "snapshot:%zu package relations", snapshot.relations.size());
  const size_t controlValueHave = readSizeValue(s);
  const size_t controlValueShouldBe = stringBufSize + nameCount + namesBufSize + snapshot.pkgs.size() + snapshot.relations.size();
  if (controlValueShouldBe != controlValueHave)
    {
      logMsg(LOG_ERR, "Control values do not match: %zu have but %zu should be", controlValueHave, controlValueShouldBe);
      THROW_INTERNAL_ERROR;//FIXME:More information;
    } else
    logMsg(LOG_DEBUG, "snapshot:control values are correct (both are %zu)", controlValueHave);
  if (snapshot.pkgs.empty())
    {
      logMsg(LOG_DEBUG, "snapshot:there are no packages, leaving package snapshot empty");
      return;
    }
  //Reading all version and release strings;
  strings.push_back(new char[stringBufSize]);
  char* stringBuf = (char*)strings.back();
  readBuf(s, stringBuf, stringBufSize );
  //Reading names of packages and provides;
  snapshot.pkgNames.reserve(nameCount);
  readNames(snapshot, s, namesBufSize);
  if(snapshot.pkgNames.size() != nameCount)
    {
      logMsg(LOG_ERR, "The number of read names does not match expected value, read %zu but expected %zu", snapshot.pkgNames.size(), nameCount);
      THROW_INTERNAL_ERROR;//FIXME:More information;
    }
  //Reading package list;
  for(Deepsolver::PkgSnapshot::PkgVector::size_type i = 0;i < snapshot.pkgs.size();i++)
    {
      Deepsolver::PkgSnapshot::Pkg& newEntry = snapshot.pkgs[i];
      newEntry.pkgId = readSizeValue(s);
      newEntry.epoch = readUnsignedShortValue(s);
      const size_t verOffset = readSizeValue(s);
      assert(verOffset < stringBufSize);//FIXME:must be an exception;
      newEntry.ver = stringBuf + verOffset;
      const size_t releaseOffset = readSizeValue(s);
      assert(releaseOffset < stringBufSize);//FIXME:must be an exception;
      newEntry.release = stringBuf + releaseOffset;
      newEntry.buildTime = readSizeValue(s);
      newEntry.requiresPos = readSizeValue(s);
      newEntry.requiresCount = readSizeValue(s);
      newEntry.providesPos = readSizeValue(s);
      newEntry.providesCount = readSizeValue(s);
      newEntry.conflictsPos = readSizeValue(s);
      newEntry.conflictsCount = readSizeValue(s);
      newEntry.obsoletesPos = readSizeValue(s);
      newEntry.obsoletesCount = readSizeValue(s);
      newEntry.flags = 0;
    }
  //Reading package relations;
  for(RelationVector::size_type i = 0;i < snapshot.relations.size();i++)
    {
      Relation& newEntry = snapshot.relations[i];
      newEntry.pkgId = readSizeValue(s);
      newEntry.verDir = readCharValue(s);
      const size_t verOffset = readSizeValue(s);
      if (verOffset != (size_t)-1)
	{
	  assert(verOffset < stringBufSize);//FIXME:must be an exception;
	  newEntry.ver = stringBuf + verOffset;
	} else
	newEntry.ver = NULL;
    }
}

void readNames(Snapshot& snapshot,
	       std::ifstream& s,
	       size_t namesBufSize)
{
  std::string chunk;
  logMsg(LOG_DEBUG, "snapshot:reading names, namesBufSize=%zu", namesBufSize);
  size_t count = 0;
  char buf[DEEPSOLVER_IO_BLOCK_SIZE + 1];
  buf[DEEPSOLVER_IO_BLOCK_SIZE] = '\0';
  while(count < namesBufSize)
    {
      const size_t toRead = (namesBufSize - count) > DEEPSOLVER_IO_BLOCK_SIZE?DEEPSOLVER_IO_BLOCK_SIZE:(namesBufSize - count);
      s.read(buf, toRead);//FIXME:Checking;
  assert(s);//FIXME:must be an exception;
  buf[toRead] = '\0';
  size_t fromPos = 0;
      for(size_t i = 0;i < toRead;i++)
	{
	  assert(fromPos < toRead && i < toRead && fromPos <= i);
	  if (buf[i] != '\0')
	    continue;
	  onNewName(snapshot, buf + fromPos, 1, chunk);//1 means it is a complete value;
	      fromPos = i + 1;
	} //for(buf);
      assert(fromPos <= toRead);
      if (fromPos < toRead)//We have incomplete value;
	onNewName(snapshot, buf + fromPos, 0, chunk);//0 means it is an incomplete value;
      count += toRead;
    }
  assert(count == namesBufSize);
  if (!chunk.empty())
    {
      logMsg(LOG_ERR, "Non-empty name chunk after names reading \'%s\', already have %zu complete names", chunk.c_str(), snapshot.pkgNames.size());
      THROW_INTERNAL_ERROR;//FIXME:More information;
    }
}

void onNewName(Snapshot& snapshot,
	       const char* name,
	       bool complete,
	       std::string& chunk)
{
  assert(name);
  if (!complete)
    {
      chunk += name;
      return;
    }
  snapshot.pkgNames.push_back(chunk + name);
  chunk.erase();
}

void saveToFile(const Snapshot& snapshot,
		const std::string& fileName,
		ConstCharVector& strings)
{
  assert(!fileName.empty());
  logMsg(LOG_DEBUG, "snapshot:starting saving package snapshot to binary file \'%s\'", fileName.c_str());
  SizeVector stringOffsets;
  stringOffsets.resize(strings.size());
  size_t k = 0;
  for(ConstCharVector::size_type i = 0;i < strings.size();i++)
    {
      stringOffsets[i] = k;
      assert(strings[i] != NULL);
      k += (strlen(strings[i]) + 1);
    }
  std::ofstream s(fileName.c_str());
  if (!s.is_open())
    {
      logMsg(LOG_ERR, "An error occurred while opening \'%s\' for writing packages snapshot", fileName.c_str());
      THROW_INTERNAL_ERROR;//FIXME:More information;
    }
  assert(strings.size() == stringOffsets.size());
  //Saving string buffer length;
  size_t controlValue = 0;
  writeSizeValue(s, k);//we must have the total length of all strings here;
  logMsg(LOG_DEBUG, "snapshot:%zu bytes in all version string constants with trailing zeroes", k);
  controlValue += k;
  writeSizeValue(s, snapshot.pkgNames.size());
  logMsg(LOG_DEBUG, "snapshot:%zu package names", snapshot.pkgNames.size());
  controlValue += snapshot.pkgNames.size();
  size_t totalNamesLen = 0;
  for(StringVector::size_type i = 0;i < snapshot.pkgNames.size();i++)
    totalNamesLen += snapshot.pkgNames[i].length() + 1;
  writeSizeValue(s, totalNamesLen);
  logMsg(LOG_DEBUG, "snapshot:%zu bytes in all package names with trailing zeroes", totalNamesLen);
  controlValue += totalNamesLen;
  writeSizeValue(s, snapshot.pkgs.size());
  logMsg(LOG_DEBUG, "snapshot:%zu packages", snapshot.pkgs.size());
  controlValue += snapshot.pkgs.size();
  writeSizeValue(s, snapshot.relations.size());
  logMsg(LOG_DEBUG, "snapshot:%zu package relations", snapshot.relations.size());
  controlValue += snapshot.relations.size();
  writeSizeValue(s, controlValue);
  logMsg(LOG_DEBUG, "snapshot:saved control value %zu", controlValue);
  if (snapshot.pkgs.empty())
    {
      logMsg(LOG_DEBUG, "snapshot:There are no packages, nothing to save");
      return;
    }
  //Saving all version and release strings;
  for(ConstCharVector::size_type i = 0;i < strings.size();i++)
    writeStringValue(s, strings[i]);
  //Saving names of packages;
  for(StringVector::size_type i = 0;i < snapshot.pkgNames.size();i++)
    writeStringValue(s, snapshot.pkgNames[i].c_str());
  //Saving package list;
  for(Deepsolver::PkgSnapshot::PkgVector::size_type i = 0;i < snapshot.pkgs.size();i++)
    {
      const Deepsolver::PkgSnapshot::Pkg& pkg = snapshot.pkgs[i];
      assert(strings[pkg.aux] == pkg.ver);
      assert(strings[pkg.aux + 1] == pkg.release);
      writeSizeValue(s, pkg.pkgId);
      writeUnsignedShortValue(s, pkg.epoch);
      writeSizeValue(s, stringOffsets[pkg.aux]);
      writeSizeValue(s, stringOffsets[pkg.aux+ 1]);
      writeSizeValue(s, pkg.buildTime);
      writeSizeValue(s, pkg.requiresPos);
      writeSizeValue(s, pkg.requiresCount);
      writeSizeValue(s, pkg.providesPos);
      writeSizeValue(s, pkg.providesCount);
      writeSizeValue(s, pkg.conflictsPos);
      writeSizeValue(s, pkg.conflictsCount);
      writeSizeValue(s, pkg.obsoletesPos);
      writeSizeValue(s, pkg.obsoletesCount);
    }
  //Saving package relations;
  for(Deepsolver::PkgSnapshot::RelationVector::size_type i = 0;i < snapshot.relations.size();i++)
    {
      const Deepsolver::PkgSnapshot::Relation& rel = snapshot.relations[i];
      assert(rel.aux == (size_t)-1 || strings[rel.aux] == rel.ver);
      writeSizeValue(s, rel.pkgId);
      writeCharValue(s, rel.verDir);
      if (rel.aux != (size_t)-1)
	writeSizeValue(s, stringOffsets[rel.aux]); else
	writeSizeValue(s, (size_t)-1);
    }
  s.flush();
}

void removeEqualPkgs(Snapshot& snapshot)
{
  PkgSnapshot::PkgVector& pkgs = snapshot.pkgs;
  if (pkgs.empty())
    return;
  PkgSnapshot::PkgVector::size_type checkFrom = 0;
  for(PkgSnapshot::PkgVector::size_type i = 1;i < pkgs.size();i++)
    {
      assert(checkFrom < pkgs.size());
      assert(pkgs[checkFrom].pkgId != BadPkgId && pkgs[i].pkgId != BadPkgId);
      if (pkgs[checkFrom].pkgId != pkgs[i].pkgId)
	{
	  checkFrom = i;
	  continue;
	}
      PkgSnapshot::PkgVector::size_type k;
      for(k = checkFrom;k < i;k++)
	{
	  if (pkgs[k].pkgId == BadPkgId)
	    continue;
	  assert(pkgs[i].pkgId == pkgs[k].pkgId);
	  assert(pkgs[k].ver != NULL && pkgs[k].release != NULL);
	  if (strcmp(pkgs[i].ver, pkgs[k].ver) == 0 &&
	      strcmp(pkgs[i].release, pkgs[k].release) == 0 &&
	      pkgs[i].buildTime == pkgs[k].buildTime)
	    break;
	}
      if (k < i)
	pkgs[i].pkgId = BadPkgId;
    }
  size_t offset = 0;
  for(PkgSnapshot::PkgVector::size_type i = 0;i < pkgs.size();i++)
    {
      if (pkgs[i].pkgId == BadPkgId)
	offset++; else
	pkgs[i - offset] = pkgs[i];
    }
  assert(offset < pkgs.size());
  pkgs.resize(pkgs.size() - offset);
  logMsg(LOG_DEBUG, "snapshot:%zu doubled packages are filtered out", offset);
}

void printContent(const Snapshot& snapshot, 
		  bool withIds,
		  std::ostream& s)
{
  for(PkgVector::size_type i = 0;i < snapshot.pkgs.size();i++)
    {
      const PkgSnapshot::Pkg& p = snapshot.pkgs[i]; 
      assert(p.pkgId != BadPkgId);
      if (withIds)
	s << "#" << i << ((p.flags & PkgFlagInstalled)?", installed: ": ": "); else
	s << ((p.flags & PkgFlagInstalled)?"Installed package: ":"Package: ");
      s << pkgIdToStr(snapshot, p.pkgId) << "-";
      if (p.epoch > 0)
	s << p.epoch << ":";
      assert(p.ver && p.release);
      s << p.ver << "-" << p.release;
      s << " (BuildTime: " << p.buildTime << ")" << std::endl;
      printRelations(snapshot, "Requires:", s, p.requiresPos, p.requiresCount);
      printRelations(snapshot, "Provides:", s, p.providesPos, p.providesCount);
      printRelations(snapshot, "Conflicts:", s, p.conflictsPos, p.conflictsCount);
      printRelations(snapshot, "Obsoletes:", s, p.obsoletesPos, p.obsoletesCount);
    }
}

void printRelations(const Snapshot& snapshot,
		    const std::string& title,
		    std::ostream& s,
		    RelationVector::size_type pos,
		    RelationVector::size_type count)
{
  for(RelationVector::size_type k = 0;k < count;k++)
    {
      assert(k + pos < snapshot.relations.size());
      const Relation& r = snapshot.relations[k + pos];
      s << title <<"  " << pkgIdToStr(snapshot, r.pkgId);
      if (r.verDir != VerNone)
	{
	  assert(r.ver);
	  s << " ";
	  if (r.verDir & VerLess)
	    s << "<";
	  if (r.verDir & VerGreater)
	    s << ">";
	  if (r.verDir & VerEquals)
	    s << "=";
	  s << " " << r.ver;
	}
      s << std::endl;
    }
}

size_t getScore(const Snapshot& snapshot)
{
  size_t value = 0;
  for(StringVector::size_type i = 0;i < snapshot.pkgNames.size();i++)
    value += snapshot.pkgNames[i].length();
  for(PkgSnapshot::PkgVector::size_type i = 0;i < snapshot.pkgs.size();i++)
    {
      const PkgSnapshot::Pkg& pkg = snapshot.pkgs[i];
      value += pkg.pkgId % 32;
      assert(pkg.ver != NULL && pkg.release != NULL);
      value += strlen(pkg.ver);
      value += strlen(pkg.release);
    }
  for(PkgSnapshot::RelationVector::size_type i = 0;i < snapshot.relations.size();i++)
    {
      const PkgSnapshot::Relation& r = snapshot.relations[i];
      value += (r.pkgId % 32) + 1;
      assert(r.verDir == VerNone || r.ver != NULL);
      assert(r.verDir != VerNone || r.ver == NULL);
      if (r.verDir == VerNone)
	value--; else
	value += strlen(r.ver);
    }
  return value;
}

bool theSameVersion(const Deepsolver::Pkg& p1, const Deepsolver::PkgSnapshot::Pkg& p2)
{
  return (p1.version == p2.ver &&
	  p1.release == p2.release &&
	  p1.buildTime == p2.buildTime);
}

DEEPSOLVER_END_PKG_SNAPSHOT_NAMESPACE
DEEPSOLVER_END_NAMESPACE
