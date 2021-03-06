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
#include"deepsolver/RpmInstalledPackagesIterator.h"
#include"deepsolver/rpmHeader.h"

DEEPSOLVER_BEGIN_NAMESPACE

void RpmInstalledPkgIterator::openEnum()
{
  if (rpmdbOpen( "", &m_db, O_RDONLY, 0644 ) != 0)//FIXME:root directory;
    throw PkgBackEndException("rpmdbOpen()");
  m_it = rpmdbInitIterator(m_db, RPMDBI_PACKAGES, NULL, 0);
}

bool RpmInstalledPkgIterator::moveNext(Pkg& pkg)
{                                                                                                                                                              
  Header h = rpmdbNextIterator(m_it);
  if (!h)
    {
      rpmdbFreeIterator(m_it);                                                                                                                                     
      rpmdbClose(m_db);                                                                                                                                            
      return 0;
    }
  rpmFillMainData(h, pkg);
  rpmFillProvides(h, pkg.provides);
  rpmFillRequires(h, pkg.requires);
  rpmFillObsoletes(h, pkg.obsoletes);
  rpmFillConflicts(h, pkg.conflicts);
  rpmFillFileList(h, pkg.fileList);
  return 1;
}                                                                                                                                                              


DEEPSOLVER_END_NAMESPACE
