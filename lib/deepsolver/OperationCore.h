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
#include"deepsolver/AbstractOperationContinueRequest.h"
#include"deepsolver/AbstractFetchListener.h"
#include"deepsolver/AutoReleaseStrings.h"

namespace Deepsolver
{
  class AbstractTransactionListener
  {
  public:
    /**\brief The default constructor*/
    AbstractTransactionListener() {}

    /**\brief The destructor*/
    virtual ~AbstractTransactionListener() {}

  public:
    virtual void onPkgListProcessingBegin() = 0;
    virtual void onPkgListProcessingEnd() = 0;
}; //class abstractTransactionListener;

  /**\brief The main class for package managing
   *
   * The OperationCore class is the central class of Deepsolver project. It
   * allows every client to perform various manipulations with packages
   * including installation, removing and upgrading. In addition this class
   * takes care of information gathered about attached repositories. 
   *
   * Be careful, each method of the OperationCore class throws a number of
   * exceptions. Their exact set depends on method purpose. The main
   * structure used for configuring is a ConfigCenter class instance
   * provided by a reference to OperationCore constructor.
   *
   * According to accepted project design there are two additional classes
   * purposed for direct client using as well as OperationCore itself:
   * IndexCore and InfoCore. The former takes care about index construction
   * and the second provides various information about known
   * packages. These three classes are the main classes a end-user could be
   * interested in.
   *
   * \sa IndexCore InfoCore OperationException
   */
  class OperationCore: private AutoReleaseStrings
  {
  public:
    /**\brief The constructor
     *
     * \param [in] conf A reference to configuration data object
     */
    OperationCore(const ConfigCenter& conf): 
      m_conf(conf)  {}

    /**\brief The destructor*/
    virtual ~OperationCore() {}

  public:
    /**\brief Updates repository content information
     *
     * \throws OperationException
     * \throws CurlException
     * \throws SystemException
     */
    void fetchIndices(AbstractFetchListener& listener,
		      const AbstractOperationContinueRequest& continueRequest);

    std::auto_ptr<TransactionIterator> transaction(AbstractTransactionListener& listener, const UserTask& task);
    std::string generateSat(AbstractTransactionListener& listener, const UserTask& task);
    void printPackagesByRequire(const NamedPkgRel& rel, std::ostream& s);

  private:
    const ConfigCenter& m_conf;
  }; //class OperationCore;
} //namespace Deepsolver;

#endif //DEEPSOLVER_OPERATION_CORE_H;
