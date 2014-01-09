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

#ifndef DEEPSOLVER_TINY_FILE_DOWNLOAD_H
#define DEEPSOLVER_TINY_FILE_DOWNLOAD_H

#include"deepsolver/CurlInterface.h"

namespace Deepsolver
{
  class TinyFileDownload: private AbstractCurlDataRecipient, private AbstractCurlProgressListener
  {
  public:
    TinyFileDownload(size_t dataSizeLimit) 
      : m_dataSizeLimit(dataSizeLimit) {}

    /**\brief The destructor*/
    virtual ~TinyFileDownload() {}

public:
    void fetch(const std::string& url);

    const std::string& getContent() const
    {
      return m_content;
    }

  private://AbstractCurlDataRecipient;
    size_t onNewDataBlock(const void* buf, size_t bufSize);

  private://AbstractCurlProgressListener;
    bool onCurlProgress(size_t now, size_t total);

  private:
    std::string m_content;
    const size_t m_dataSizeLimit;
  }; //class TinyFileDownload;
} //namespace Deepsolver;

#endif //DEEPSOLVER_TINY_FILE_DOWNLOAD_H;
