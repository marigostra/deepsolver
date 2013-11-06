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

#ifndef DEEPSOLVER_SOLVER_BASE_H
#define DEEPSOLVER_SOLVER_BASE_H

#define DEEPSOLVER_BEGIN_SOLVER_NAMESPACE namespace Deepsolver { namespace Solver{
#define DEEPSOLVER_END_SOLVER_NAMESPACE }}

namespace Deepsolver
{
  namespace Solver
  {
    class AbstractPkgScope
    {
    public:
      enum {
	EpochNever,
	EpochIfNonZero,
	EpochAlways
      };

    public:
      AbstractPkgScope() {}
      virtual ~AbstractPkgScope() {}

    public:
      virtual bool isInstalled(VarId varId) const = 0;
      virtual void getConflicts(VarId varId, IdPkgRelVector& res) const = 0;
      virtual void getRequires(VarId varId, IdPkgRelVector& res) const = 0;
      virtual bool allProvidesHaveTheVersion(const VarIdVector& vars, PkgId providePkgId) const = 0;
      virtual void selectMatchingVarsProvidesOnly(const IdPkgRel& rel, VarIdVector& vars) const = 0;
      virtual void selectMatchingVarsProvidesOnly(PkgId pkgId, VarIdVector& vars) const = 0;

      virtual void selectMatchingVarsProvidesOnly(PkgId pkgId,
						   const VersionCond& ver,
						   VarIdVector& vars) const = 0;

      virtual void selectMatchingVarsRealNames(const IdPkgRel& rel, VarIdVector& vars) const = 0;
      virtual void selectMatchingVarsRealNames(PkgId pkgId, VarIdVector& vars) const = 0;

      virtual void selectMatchingVarsRealNames(PkgId pkgId,
					       const VersionCond& ver,
					       VarIdVector& vars) const = 0;

      virtual void selectMatchingVarsWithProvides(const IdPkgRel& rel, VarIdVector& vars) const = 0;
      virtual void selectMatchingVarsWithProvides(PkgId pkgId, VarIdVector& vars) const = 0;

      virtual void selectMatchingVarsWithProvides(PkgId pkgId,
						  const VersionCond& ver,
						  VarIdVector& vars) const = 0;

      virtual void selectTheNewest(VarIdVector& vars) const = 0;
      virtual void selectTheNewestByProvide(VarIdVector& vars, PkgId provideEntry) const = 0;

      virtual void whatConflictAmongInstalled(VarId varId,
					      VarIdVector& res,
					      IdPkgRelVector& resRels) const = 0;

      virtual void whatDependAmongInstalled(VarId varId,
					    VarIdVector& res,
					    IdPkgRelVector& resRels) const = 0;

      virtual void whatSatisfyAmongInstalled(const IdPkgRel& rel, VarIdVector& res) const = 0;

    public:
      virtual size_t getPkgCount() const = 0;
      virtual bool knownPkgName(const std::string& name) const = 0;
      virtual std::string getPkgName(VarId varId) const = 0;
      virtual std::string getDesignation(VarId varId, int epochMode) const = 0;
      virtual std::string getDesignation(const IdPkgRel& r) const = 0;

      virtual std::string getVersion(VarId varId, int epochMode) const = 0;
      virtual void fullPkgData(VarId varId, Pkg& pkg) const = 0;
      virtual PkgId pkgIdOfVarId(VarId varId) const = 0;
      virtual std::string pkgIdToStr(PkgId pkgId) const = 0;
      virtual PkgId strToPkgId(const std::string& name) const = 0;

    public:
      std::string getDesignation(VarId varId) const
      {
	return getDesignation(varId, EpochIfNonZero);
      }

      std::string getVersion(VarId varId) const
      {
	return getVersion(varId, EpochIfNonZero);
      }
    }; //class AbstractPkgScope;

    class AbstractProvidePriority
    {
    public:
      AbstractProvidePriority() {}
      virtual ~AbstractProvidePriority() {}

    public:
      virtual VarId byProvidesPriorityList(const VarIdVector& vars, PkgId provideEntry) const = 0;
    }; //class AbstractProvidePriority;
  } //namespace Solver;

  using Solver::AbstractPkgScope;
} //namespace Deepsolver;

#endif //DEEPSOLVER_SOLVER_BASE_H;
