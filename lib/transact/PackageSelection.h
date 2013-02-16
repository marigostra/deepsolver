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

#ifndef DEEPSOLVER_PACKAGE_SELECTION_H
#define DEEPSOLVER_PACKAGE_SELECTION_H

#include"AbstractPackageBackEnd.h"
#include"AbstractPackageScope.h"
#include"AbstractTaskSolver.h"

class PackageSelection
{
public:
  PackageSelection(const AbstractPackageBackEnd& backEnd,
		   const AbstractPackageScope& scope,
		   const TaskSolverProvideInfoVector& providesPriority)
    : m_backEnd(backEnd),
      m_scope(scope),
      m_providesPriority(providesPriority) {}

  virtual ~PackageSelection() {}

public:
  VarId translateItemToInstall(const UserTaskItemToInstall& item) const;
  VarId satisfyRequire(const IdPkgRel& rel) const ;

private:
  VarId satisfyRequire(PackageId pkgId) const;
  VarId satisfyRequire(PackageId pkgId, const VersionCond& version) const;
  VarId processProvidesPriorityList(const VarIdVector& vars, PackageId provideEntry) const;
  VarId processProvidesPriorityBySorting(const VarIdVector& vars) const;

private:
  const AbstractPackageBackEnd& m_backEnd;
  const AbstractPackageScope& m_scope;
  const TaskSolverProvideInfoVector& m_providesPriority;
}; //class PackageSelection;

#endif //DEEPSOLVER_PACKAGE_SELECTION_H;
