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

#ifndef DEEPSOLVER_FILES_FETCH_PROGRESS_H
#define DEEPSOLVER_FILES_FETCH_PROGRESS_H

#include"deepsolver/AbstractFetchListener.h"

namespace Deepsolver
{
  class FilesFetchProgress: public AbstractFetchListener
  {
  public:
    FilesFetchProgress(std::ostream& stream, bool suppressed)
      : m_stream(stream), 
	m_prevStrLen(0),
	m_suppressed(suppressed) {}

    virtual ~FilesFetchProgress() {}

  public:
    void onHeadersFetch();
    void onFetchBegin();
    void onFilesReading();
    void onFetchIsCompleted();
    void onFetchStatus(unsigned char currentPartPercents,
		       unsigned char totalPercents,
		       size_t partNumber,
		       size_t partCount,
		       size_t currentPartSize,
		       const std::string& currentPartName);

  private:
    std::ostream& m_stream;
    std::string::size_type m_prevStrLen;
    bool m_suppressed;
  }; //class FilesFetchProgress;
} //namespace Deepsolver;

#endif //DEEPSOLVER_FILES_FETCH_PROGRESS_H;
