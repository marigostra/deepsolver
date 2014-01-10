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

#ifndef DEEPSOLVER_ABSTRACT_PKG_BACK_END_H
#define DEEPSOLVER_ABSTRACT_PKG_BACK_END_H

namespace Deepsolver
{
  /**\brief The abstract interface for iterator over a set of installed packages
   *
   * This class isolates any engine of particular package library, which can
   * be used to read database with installed packages data. Its instance should
   * be obtained through the methods of AbstractPackageBackEnd class.
   *
   * \sa AbstractPackageBackEnd RpmBackEnd RpmInstalledPackagesIterator
   */
  class AbstractInstalledPkgIterator
  {
  public:
    typedef std::shared_ptr<AbstractInstalledPkgIterator> Ptr;

  public:
    /**\brief The default constructor*/
    AbstractInstalledPkgIterator() {}

    /**\brief The destructor*/
    virtual ~AbstractInstalledPkgIterator() {}

  public:
    /**\brief Goes to next installed package and fills data associated with it
     *
     * After creation the iterator is pointed before the first element. Each
     * invocation of this method changes position to next package if there is
     * any and fills necessary data. If there is no next package (including
     * the case there are no packages at all) this method returns zero.
     *
     * \param [out] pkg The Pkg class instance to received data
     *
     * \return Non-zero if there is the next package or zero otherwise (including the case there are no packages at all)
     */
    virtual bool moveNext(Pkg& pkg) = 0; 
  }; //class AbstractInstalledPkgIterator;

  /**\brief The abstract package back-end interface
   *
   * This abstract interface isolates any functions of an particular
   * package library like rpm, dpkg or any other. They can be treated as
   * back-ends for Deepsolver itself.  Although all methods has strict
   * declaration they semantics can be not exactly the same due to
   * differences in package libraries behaviour. So every developer and
   * user should be very careful on any attempt to add new library
   * support. The first real implementation of this class was for librpm, so all
   * dependent code has unpremeditated influence of this library.
   *
   * \sa RpmBackEnd 
   */
  class AbstractPkgBackEnd
  {
  public:
      enum {
	EpochNever,
	EpochIfNonZero,
	EpochAlways
      };

  public:
    typedef std::shared_ptr<AbstractPkgBackEnd> Ptr;

  public:
    /**\brief The default constructor*/
    AbstractPkgBackEnd() {}

    /**\brief The destructor*/
    virtual ~AbstractPkgBackEnd() {}

  public:
    /**\brief Prepares package back-end for any operations
     *
     * This method should be called before performing any operations with
     * particular package library. For safety reasons it is assumed that 
     * invocation is needed for every created instance of the back-end but
     * actually it is not always the case. For example, if RpmBackEnd is used, this
     * method may be called only once using any instance.
     */
    virtual void initialize() = 0;

    /**\brief Compares two version strings
     *
     * \param [in] ver1 The first version value to compare
     * \param [in] ver2 The second version value to compare
     *
     * \return The integer value less than zero if ver1 less than ver2, greater than zero if ver1 greater than ver2 and zero otherwise
     */
    virtual int verCmp(const std::string& ver1, const std::string& ver2) const = 0;

    /**\brief Overlaps two version ranges
     *
     * This method is not symmetric. If second version range has no epoch
     * indication it assumes the first one may have any. So if this
     * method is used for requires processing, the require entry can go
     * only as second argument.
     *
     * \param [in] ver1 The first version range to intersect
     * \param [in] ver2 The second version range to intersect
     *
     * \return Non-zero if intersection is not empty and zero otherwise
     */
    virtual bool verOverlap(const VerSubset& ver1, const VerSubset& ver2) const = 0;

    /**\brief Checks the equality of two version values
     *
     * \param [in] ver1 The first string to compare
     * \param [in] ver2 The second string to compare
     *
     * \return Non-zero if two strings designate the same value or zero otherwise
     */
    virtual bool verEqual(const std::string& ver1, const std::string& ver2) const = 0;

    /**\brief Checks if one version is newer than another
     *
     * \param [in] ver1 The first string to compare
     * \param [in] ver2 The second string to compare
     *
     * \return Non-zero if ver1 is greater than ver2 or zero if ver1 is less or equal ver2
     */
    virtual bool verGreater(const std::string& ver1, const std::string& ver2) const = 0;

    /**\brief Creates an instance of an iterator over the set of installed packages
     * \return The iterator over the set of installed packages
     */
    virtual AbstractInstalledPkgIterator::Ptr enumInstalledPkg() const = 0;

    /**\brief Reads header information from package file on disk
     *
     * \param [in] fileName The name of the file to read data from
     * \param [out] pkgFile The object to save retrieved data to
     */
    virtual void readPkgFile(const std::string& fileName, PkgFile& pkgFile) const = 0;

    /**\brief Checks if provided file name is a proper package name
     *
     * \param [in] fileName The file name to check
     *
     * \return Non-zero if provided file name is a valid package name and zero otherwise
     */
    virtual bool validPkgFileName(const std::string& fileName) const = 0;

    /**\brief Checks if provided file name is a proper source package name
     *
     * \param [in] fileName The file name to check
     *
     * \return Non-zero if provided file name is a valid source package name and zero otherwise
     */
    virtual bool validSourcePkgFileName(const std::string& fileName) const = 0;

    /**\brief Performs install/remove transaction with given packages
     *
     * This method gives a way to make desired changes in operating system
     * state. With it everybody can perform a transaction containing any
     * installation, removing, upgrading and downgrading tasks. The main
     * restriction is a requirement that all package dependencies and
     * conflicts must be satisfied. The package to install must be provided
     * with their file names, packages to remove by their names (without a
     * version or any other additional information), packages to upgrade and
     * downgrade must be specified by a string-to-string map from package
     * names to file names.
     *
     * If a transaction fails this method returns zero. The state of OS after
     * failed transaction is unspecified and package back-end dependent.
     *
     * \param [in] toInstall The file names vector with packages to install
     * \param [in] toRemove The file names vector with packages to remove
     * \param [in] toUpgrade The map from package names to file names with packages to upgrade
     * \param [in] toDowngrade The map from package names to file names with packages to downgrade
     *
     * \return Non-zero if a transaction is completed successfully or zero otherwise
     *
     * \throws PackageBackEndException
     */
    virtual bool transaction(const StringVector& toInstall,
			     const StringVector& toRemove,
			     const StringToStringMap& toUpgrade,
			     const StringToStringMap& toDowngrade) = 0;

    /**\brief Constructs a version designating string
     *
     * \param [in] epoch The version epoch
     * \param [in ver The package version
     * \param [in]] The package release
     * \param [in] The epoch including mode (can be EpochNever, EpochIfNonZero or EpochAlways)
     *
     * \return The constructed version string
     */
    virtual std::string makeVer(int epoch,
			      const std::string& ver,
			      const std::string& release,
			      int epochMode) const = 0;

    /**\brief Constructs the string designating the package version
     *
     * \param [in] pkg The package to construct version string for
     * \param [in] The epoch including mode (can be EpochNever, EpochIfNonZero or EpochAlways)
     *
     * \return The version string for the provided package
     */
    virtual std::string makeVer(const PkgBase& pkg, int epochMode) const = 0;

    /**\brief Combines the package name and the package version into one string
     *
     * \param [in] name The package name
     * \param [in] ver The package version
     *
     *
     * \return The combined  package string
     */
    virtual std::string combineNameAndVer(const std::string& name, const std::string& ver) const = 0;

    /**\brief Constructs the string designation of the provided package
     *
     * \param [in] pkg The package to construct string designation for
     * \param [in] The epoch including mode (can be EpochNever, EpochIfNonZero or EpochAlways)
     *
     * \return The constructed package designation
     */
    virtual std::string getDesignation(const PkgBase& pkg, int epochMode) const = 0;

    /**\brief Constructs the string designation of the package relation
     * \param [in] rel The package relation to construct designation for
     *
     * \return The constructed package relation designation
     */
    virtual std::string getDesignation(const NamedPkgRel& rel) const = 0;

    /**\brief Checks if the package suits to the relation
     *
     * \param [in] rel The relation
     * \param [in] pkg The package to check for
     *
     * \return Non-zero if the package matches the relation and zero otherwise
     */
    virtual bool matches(const NamedPkgRel& rel, const Pkg& pkg) const = 0;

    /**\brief Checks if the provide entry suits to the relation (named form)
     *
     * \param [in] rel The relation to check with
     * \param [in] provide The provide entry to check
     *
     * \return Non-zero if the provide entry matches the relation
     */
    virtual bool matches(const NamedPkgRel& rel, const NamedPkgRel& provide) const = 0;

    /**\brief Checks if the provide entry suits to the relation (IDs form)
     *
     * \param [in] rel The relation to check with
     * \param [in] provide The provide entry to check
     *
     * \return Non-zero if the provide entry matches the relation
     */
    virtual bool matches(const IdPkgRel& rel, const IdPkgRel& provide) const = 0;
  }; //class AbstractPkgBackEnd;

  AbstractPkgBackEnd::Ptr createRpmBackEnd();

  //General proposal is to consider this line as main switch what backend we want;
#define CREATE_PKG_BACKEND createRpmBackEnd()
} //namespace Deepsolver;

#endif //DEEPSOLVER_ABSTRACT_PKG_BACK_END_H;
