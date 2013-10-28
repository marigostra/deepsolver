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

#ifndef DEEPSOLVER_RPM_BACKEND_H
#define DEEPSOLVER_RPM_BACKEND_Hs

#include"deepsolver/AbstractPkgBackEnd.h"
#include"deepsolver/RpmInstalledPackagesIterator.h"
//FIXME:#include"rpm/Rpmdb.h"
//FIXME:#include"rpm/RpmFile.h"

namespace Deepsolver
{
  /**\brief The general interface to librpm functions
   *
   * This class covers all functions of librpm needed for proper Deepsolver
   * work. Since Deepsolver purposed by design as universal package manager
   * all package libraries should be used through an abstraction layer
   * represented by AbstractPackageBackEnd class. Be careful, even with
   * abstraction layer user can face compatibility problems due to
   * differences in details implementation. 
   *
   * \sa AbstractPackageBackEnd
   */
  class RpmBackEnd: public AbstractPkgBackEnd
  {
  public:
    /**\brief The default constructor*/
    RpmBackEnd() {}

    /**\brief The destructor*/
    virtual ~RpmBackEnd() {}

  public:
    void initialize() override;
    int verCmp(const std::string& ver1, const std::string& ver2) const override;
    bool verOverlap(const VersionCond& ver1, const VersionCond& ver2) const override;
    bool verEqual(const std::string& ver1, const std::string& ver2) const override;
    bool verGreater(const std::string& ver1, const std::string& ver2) const override;
    std::auto_ptr<AbstractInstalledPkgIterator> enumInstalledPkg() const override;
    void readPkgFile(const std::string& fileName, PkgFile& pkgFile) const override;
    bool validPkgFileName(const std::string& fileName) const override;
    bool validSourcePkgFileName(const std::string& fileName) const override;

    bool transaction(const StringVector& toInstall,
		     const StringVector& toRemove,
		     const StringToStringMap& toUpgrade,
		     const StringToStringMap& toDowngrade) override;
  }; //class RpmBackEnd;
} //namespace Deepsolver;

#endif //DEEPSOLVER_RPM_BACKEND_H;
