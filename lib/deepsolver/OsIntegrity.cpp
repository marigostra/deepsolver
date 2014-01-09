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
#include"deepsolver/OsIntegrity.h"

DEEPSOLVER_BEGIN_NAMESPACE

bool OsIntegrity::verify(const PkgVector& pkgs) const
{
  for(PkgVector::size_type i = 0;i < pkgs.size();++i)
    {
      const Pkg& p = pkgs[i];
      for(NamedPkgRelVector::size_type k = 0;k < p.requires.size();++k)
	if (!checkRequire(p.requires[k], pkgs))
	  {
	    logMsg(LOG_ERR, "integrity:the package \'%s\' has broken require \'%s\'", p.designation().c_str(), p.requires[k].designation().c_str());
	    return 0;
	  }
      for(NamedPkgRelVector::size_type k = 0;k < p.conflicts.size();++k)
	if (!checkConflict(p.conflicts[k], pkgs))
	  {
	    logMsg(LOG_ERR, "integrity:the package \'%s\' has violated conflict \'%s\'", p.designation().c_str(), p.conflicts[k].designation().c_str());
	    return 0;
	  }
    }
  return 1;
}

bool OsIntegrity::checkRequire(const NamedPkgRel& require, const PkgVector& pkgs) const
{
  for(PkgVector::size_type i = 0;i < pkgs.size();++i)
    if (pkgs[i].name == require.pkgName)
      {
	if (!require.verRestricted())
	  return 1;
	if (m_backend.verOverlap(VerSubset(pkgs[i].fullVersion()), VerSubset(require.ver, require.type)))
	  return 1;
      }
  //Provides;
  for(PkgVector::size_type i = 0;i < pkgs.size();++i)
    for(NamedPkgRelVector::size_type k = 0;k < pkgs[i].provides.size();++k)
      {
	const NamedPkgRel& provide = pkgs[i].provides[k];
	//FIXME:Make a unified  requires-provides checking in AbstractPkgBackEnd;
	if (provide.pkgName == require.pkgName)
	  {
	    if (!require.verRestricted() && !provide.verRestricted())
	      return 1;
	    if (m_backend.verOverlap(VerSubset(provide.ver, provide.type), VerSubset(require.ver, require.type)))
	      return 1;
	  }
      }
  return 0;
}

bool OsIntegrity::checkConflict(const NamedPkgRel& conflict, const PkgVector& pkgs) const
{
  for(PkgVector::size_type i = 0;i < pkgs.size();++i)
    if (pkgs[i].name != conflict.pkgName)
      {
	if (!conflict.verRestricted())
	  return 0;
	if (m_backend.verOverlap(VerSubset(pkgs[i].fullVersion()), VerSubset(conflict.ver, conflict.type)))
	  return 0;
      }
  //Provides;
  for(PkgVector::size_type i = 0;i < pkgs.size();++i)
    if (pkgs[i].name != conflict.pkgName)
      for(NamedPkgRelVector::size_type k = 0;k < pkgs[i].provides.size();++k)
	{
	  const NamedPkgRel& provide = pkgs[i].provides[k];
	  //FIXME:Make a unified  conflicts-provides checking in AbstractPkgBackEnd;
	  if (provide.pkgName != conflict.pkgName)
	    continue;
	  if (!conflict.verRestricted() && !provide.verRestricted())
	    return 0;
	  if (m_backend.verOverlap(VerSubset(provide.ver, provide.type), VerSubset(conflict.ver, conflict.type)))
	    return 0;
	}
  return 1;
}

DEEPSOLVER_END_NAMESPACE
