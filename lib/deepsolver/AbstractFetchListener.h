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

#ifndef DEEPSOLVER_ABSTRACT_FETCH_LISTENER_H
#define DEEPSOLVER_ABSTRACT_FETCH_LISTENER_H

namespace Deepsolver 
{
  /**\brief The abstract interface to listen files fetching progress
   *
   * This abstract interface receives complete information about files
   * fetching process. This information may be used to let user know what
   * is happening in an arbitrary time moment.
   *
   * This abstract class is purposed for all files fetching tasks but some
   * its methods used only during repository index downloading. They are
   * marked explicitly.
   *
   * \sa OperationCore TransactionIterator FilesFetching
   */
  class AbstractFetchListener
  {
  public:
    /**\brief The default constructor*/
    AbstractFetchListener() {}

    /**\brief The destructor*/
    virtual ~AbstractFetchListener() {}

  public:
    /**\brief Notifies basic headers downloading is in progress (occurs only at index fetching)
     *
     * Implement this method to show notification basic headers come to machine.
     */
    virtual void onHeadersFetch() = 0;

    /**\brief Notifies fetching task is initiated
     *
     * Implement this method to catch a moment when fetching process begins.
     */
    virtual void onFetchBegin() = 0;

    /**\brief Notifies fetched files are being read (occurs only after index fetching)
     *
     * Implement this method to know fetched files reading begins.
     */
    virtual void onFilesReading() = 0;

    /**\brief Notifies files fetching process is finished
     *
     * Implement this method to know everything is done.
     */
    virtual void onFetchIsCompleted() = 0;

    /**\brief Updates a status for each file fetching
     *
     * This method is called each time new portion of data is obtained and let 
     * you know how many total percents of work are completed.
     *
     * \param [in] currentPartPercents How many percents of current part are passed
     * \param [in] totalPercents How many total percents are passed
     * \param [in] partNumber A number of current part
     * \param [in] partCount Total number of parts
     * \param [in] currentPartSize A size of current part in bytes
     * \param [in] currentPartName Name of current part (usually URL)
     */
    virtual void onFetchStatus(unsigned char currentPartPercents,
			       unsigned char totalPercents,
			       size_t partNumber,
			       size_t partCount,
			       size_t currentPartSize,
			       const std::string& currentPartName) = 0;
  }; //class AbstractFetchListener;

} //namespace Deepsolver;

#endif //DEEPSOLVER_ABSTRACT_FETCH_LISTENER_H;
