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
#include"deepsolver/PkgScopeMetadata.h"

DEEPSOLVER_BEGIN_NAMESPACE

void PkgScopeMetadata::initMetadata()
{
  const clock_t start = clock();
  fillRevMapProvides();
  fillRevMapRequires();
  fillRevMapConflicts();
  const double duration = (double)(clock() - start) / CLOCKS_PER_SEC;
  logMsg(LOG_DEBUG, "scope:metadata constructed in %f sec", duration);
  logMsg(LOG_DEBUG, "scope:the scope is initialized for %zu packages", m_pkgs.size());
  logMsg(LOG_DEBUG, "scope:revmap for provides contains %zu items", m_revMapProvides.size());
  logMsg(LOG_DEBUG, "scope:revmap for installed requires contains %zu items", m_revMapInstalledRequires.size());
  logMsg(LOG_DEBUG, "scope:revmap for installed conflicts contains %zu items", m_revMapInstalledConflicts.size());
}

void PkgScopeMetadata::fillRevMapProvides()
{
  for(SnapshotPkgVector::size_type i = 0;i < m_pkgs.size();++i)
    {
      const size_t pos = m_pkgs[i].providesPos; 
      const size_t count = m_pkgs[i].providesCount;
      for(size_t k = 0;k < count;++k)
	m_revMapProvides.push_back(RevMapItem(m_relations[pos + k].pkgId, i));
    }
  std::sort(m_revMapProvides.begin(), m_revMapProvides.end());
}

void PkgScopeMetadata::fillRevMapRequires()
{
  for(SnapshotPkgVector::size_type i = 0;i < m_pkgs.size();++i)
    if (m_pkgs[i].flags & PkgFlagInstalled)
      {
	const size_t pos = m_pkgs[i].requiresPos; 
	const size_t count = m_pkgs[i].requiresCount;
	for(size_t k = 0;k < count;++k)
	  m_revMapInstalledRequires.push_back(RevMapItem(m_relations[pos + k].pkgId, i));
      }
  std::sort(m_revMapInstalledRequires.begin(), m_revMapInstalledRequires.end());
}

void PkgScopeMetadata::fillRevMapConflicts()
{
  for(SnapshotPkgVector::size_type i = 0;i < m_pkgs.size();++i)
    if (m_pkgs[i].flags & PkgFlagInstalled)
      {
	const size_t pos = m_pkgs[i].conflictsPos; 
	const size_t count = m_pkgs[i].conflictsCount;
	for(size_t k = 0;k < count;++k)
	  m_revMapInstalledConflicts.push_back(RevMapItem(m_relations[pos + k].pkgId, i));
      }
  std::sort(m_revMapInstalledConflicts.begin(), m_revMapInstalledConflicts.end());
}

void PkgScopeMetadata::findProviders(PkgId providePkgId, VarIdVector& res) const
{
  assert(providePkgId != BadPkgId);
  res.clear();
  RevMap::size_type fromPos = 0, toPos = 0;
  if (!Dichotomy<RevMapItem>().findMultiple(m_revMapProvides, RevMapItem(providePkgId, BadVarId), fromPos, toPos))
    return;
  for(RevMap::size_type i = fromPos;i < toPos;++i)
    {
      assert(m_revMapProvides[i].first == providePkgId);
      res.push_back(m_revMapProvides[i].second);
    }
}

void PkgScopeMetadata::findPkgsByRequire(PkgId requirePkgId, VarIdVector& res) const
{
  assert(requirePkgId != BadPkgId);
  res.clear();
  RevMap::size_type fromPos = 0, toPos = 0;
  if (!Dichotomy<RevMapItem>().findMultiple(m_revMapInstalledRequires, RevMapItem(requirePkgId, BadVarId), fromPos, toPos))
    return;
  for(RevMap::size_type i = fromPos;i < toPos;++i)
    {
      assert(m_revMapInstalledRequires[i].first == requirePkgId);
      res.push_back(m_revMapInstalledRequires[i].second);
    }
}

void PkgScopeMetadata::findPkgsByConflict(PkgId conflictPkgId, VarIdVector& res) const
{
  assert(conflictPkgId != BadPkgId);
  res.clear();
  RevMap::size_type fromPos = 0, toPos = 0;
  if (!Dichotomy<RevMapItem>().findMultiple(m_revMapInstalledConflicts, RevMapItem(conflictPkgId, BadVarId), fromPos, toPos))
    return;
  for(RevMap::size_type i = fromPos;i < toPos;++i)
    {
      assert(m_revMapInstalledConflicts[i].first == conflictPkgId);
      res.push_back(m_revMapInstalledConflicts[i].second);
    }
}

DEEPSOLVER_END_NAMESPACE
