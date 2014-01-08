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

#ifndef DEEPSOLVER_OPERATION_CORE_H
#define DEEPSOLVER_OPERATION_CORE_H

#include"deepsolver/ConfigCenter.h"
#include"deepsolver/TransactionIterator.h"
#include"deepsolver/AbstractContinueRequest.h"
#include"deepsolver/AbstractTransactionListener.h"
#include"deepsolver/AutoReleaseStrings.h"

namespace Deepsolver
{
  /**\brief The main class for package managing
   *
   * The OperationCore class is the central class of Deepsolver project. It
   * allows every client to perform various manipulations with packages,
   * including installation, removing and upgrading. In addition, this class
   * takes care of information gathered about attached repositories. 
   *
   * Be careful, each method of the OperationCore class throws a number of
   * exceptions. Their exact set depends on particular method purpose. 
   *
   * The main structure used for configuring is a ConfigCenter class instance
   * provided by a reference to constructor.
   *
   * According to accepted project design, there are two additional classes
   * purposed for direct usage by clients. They are
   * IndexCore and InfoCore. The former takes care about repository metadata construction
   * and the second provides various information about known
   * packages. OperationCore, IndexCore and InfoCore are the main classes a end-user should be
   * interested in.
   *
   * \sa IndexCore InfoCore OperationException
   */
  class OperationCore: private AutoReleaseStrings
  {
  public:
    /**\brief The constructor
     *
     * \param [in] conf The reference to a configuration data
     */
    OperationCore(const ConfigCenter& conf): 
      m_conf(conf)  {}

    /**\brief The destructor*/
    virtual ~OperationCore() {}

  public:
    /**\brief INitiates new transaction
     *
     * Transaction here does not imply that client application really wants
     * some changes in OS. This method only initiates the process and allows
     * to understand what changes reflect particular user task, but following
     * process can be interrupted at any stage.
     *
     * \param [in] listener The reference to an object to following calculation progress
     * \param [in] task The desirable changes
     * 
     * \return Smart pointer to transaction iterator
     *
     * \throws OperationException TaskException SystemException InternalException
     */
    TransactionIterator::Ptr transaction(AbstractTransactionListener& listener, const UserTask& task);

    /**\brief Restores all dependencies for specified package set
     *
     * This method returns the changes needed for the installation of the
     * specified package set into initially empty system. Obviously, this
     * situation is artificial and never happens in practise. None changes
     * are actually made and this method just returns the list of the
     * packages to fill the empty system with.
     *
     * \param [in] toInstall The list of the packages to "install" into an empty system
     * \param [out] res The constructed list of packages with all necessary dependencies
     *
     * \throws OperationCoreException TaskException SystemException InternalProblemException
     */
    void closure(const UserTaskItemToInstallVector& toInstall, PkgVector& res);

    /**\brief Fetchs fresh metadata of attached repositories
     *
     * \param [in] listener The reference to a listener to follow fetching progress
     * \param [in] continueRequest The reference to an object for fetching interruption by user request
     *
     * \throws OperationException CurlException SystemException InternalException
     */
    void fetchMetadata(AbstractFetchListener& listener,
		      const AbstractOperationContinueRequest& continueRequest);

    /**\brief Fills the string vector with the list of all known packages
     *
     * \param [out] res The string list to save list of known package names to
     *
     * \throws OperationCoreException SystemException InternalProblemException
     */
    void getPkgNames(StringVector& res);

    /**\brief Generates SAT and saves it as a string
     *
     * \param [in] listener The reference to an object to follow calculation progress
     * \param [in] task The reference to task object to generate SAT for
     * \param [in] s The reference to a stream to save generated SAT in
     *
     * \throws OperationException TaskException SystemException InternalException
     */
    void generateSat(AbstractTransactionListener& listener,
		     const UserTask& task,
		     std::ostream& s);

    /**\brief Print  to stream the list of packages matching to a require
     *
     * \param [in] rel The require entry to search packages for
     * \param [in] s The stream to print to 
     *
     * \throws OperationCoreException SystemException InternalProblemException
     */
    void printPackagesByRequire(const NamedPkgRel& rel, std::ostream& s);

    /**\brief Dumps the current package snapshot to stream in a string form
     *
     * \param [in] withInstalled Whether to include installed packages to dump or not
     * \param [in] withIds Whether print internal package IDs or not
     * \param [in] The stream to dump to
     *
     * \throws OperationCoreException SystemException InternalProblemException
     */
    void printSnapshot(bool withInstalled, 
		       bool withIds,
		       std::ostream& s);

  private:
    const ConfigCenter& m_conf;
  }; //class OperationCore;
} //namespace Deepsolver;

#endif //DEEPSOLVER_OPERATION_CORE_H;
