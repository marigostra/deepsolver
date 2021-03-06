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

#ifndef DEEPSOLVER_FILES_FETCH_H
#define DEEPSOLVER_FILES_FETCH_H

#include"deepsolver/AbstractContinueRequest.h"
#include"deepsolver/AbstractFetchListener.h"
#include"deepsolver/CurlInterface.h"

namespace Deepsolver
{
  /**\brief The files downloading manager
   * This class is responsible for various files fetching tasks. It takes
   * list of URLs with corresponding local file names and invokes
   * CurlInterface managing requests operation continuing is permitted and
   * sending proper status callbacks. 
   *
   * \sa AbstractOperationContinueRequest AbstractFetchListener OperationCore TransactionIterator
   */
  class FilesFetch
    : private AbstractCurlDataRecipient,
      private AbstractCurlProgressListener
  {
  public:
    /**\brief The constructor
     *
     * \param [in] listener A reference to object to receive status updates
     * \param [in] continueRequest A reference to object to be asked operation continuing is permitted
     */
    FilesFetch(AbstractFetchListener& listener,   const AbstractOperationContinueRequest& continueRequest)
      : m_listener(listener),
	m_continueRequest(continueRequest) {}

    /**\brief The destructor*/
    virtual ~FilesFetch() {}

  public:
    /**\brief Performs files fetching
     *
     * Use this method to lounch downloading process.
     *
     * \param [in] files A map describing what URLs to what local files should be fetched
     */
    void fetch(const StringToStringMap& files);

  public:
    static bool isLocalFileUrl(const std::string& url);
    static bool isLocalFileUrl(const std::string& url, std::string& localFileName);

  private:
    void processFile(const std::string& url, const std::string localFile);

  private://AbstractCurlDataRecipient;
    size_t onNewDataBlock(const void* buf, size_t bufSize);

  private://AbstractCurlProgressListener;
    bool onCurlProgress(size_t now, size_t total);

  private:
    AbstractFetchListener& m_listener;
    const AbstractOperationContinueRequest& m_continueRequest;
    File m_file;
    std::string m_currentFileName;
    size_t m_currentPartNumber;
    size_t m_partCount;
    unsigned char m_lastPartPercents, m_lastTotalPercents;
  }; //class FilesFetch;
} //namespace Deepsolver;

#endif //__DEEPSOLVER_FILES_FETCH_H;
