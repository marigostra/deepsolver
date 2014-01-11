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
	    logMsg(LOG_ERR, "integrity:the package \'%s\' has broken require \'%s\'",
		   m_backend.getDesignation(p, AbstractPkgBackEnd::EpochIfNonZero).c_str(),
		   m_backend.getDesignation(p.requires[k]).c_str());
	    return 0;
	  }
      for(NamedPkgRelVector::size_type k = 0;k < p.conflicts.size();++k)
	if (!checkConflict(p, p.conflicts[k], pkgs))
	  {
	    logMsg(LOG_ERR, "integrity:the package \'%s\' has violated conflict \'%s\'",
		   m_backend.getDesignation(p, AbstractPkgBackEnd::EpochIfNonZero).c_str(),
		   m_backend.getDesignation(p.conflicts[k]).c_str());
	    return 0;
	  }
    }
  logMsg(LOG_INFO, "integrity:ok:%zu packages verified", pkgs.size());
  return 1;
}

bool OsIntegrity::checkRequire(const NamedPkgRel& require, const PkgVector& pkgs) const
{
  for(PkgVector::size_type i = 0;i < pkgs.size();++i)
    if (m_backend.matches(require, pkgs[i]))
      return 1;
  return 0;
}

bool OsIntegrity::checkConflict(const Pkg& pkg,
				const NamedPkgRel& conflict,
				const PkgVector& pkgs) const
{
  for(PkgVector::size_type i = 0;i < pkgs.size();++i)
    {
      if (m_backend.theSamePkg(pkg, pkgs[i]))
	continue;
      if (m_backend.matches(conflict, pkgs[i]))//FIXME:The package cannot conflict with itself;
	return 0;
    }
  return 1;
}

DEEPSOLVER_END_NAMESPACE
