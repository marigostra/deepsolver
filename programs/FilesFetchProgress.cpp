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

#include"deepsolver/deepsolver.h"
#include"FilesFetchProgress.h"

DEEPSOLVER_BEGIN_NAMESPACE

void FilesFetchProgress::onHeadersFetch()
{
  if (m_suppressed)
    return;
  m_stream << "Downloading basic repository data" << std::endl;
}

void FilesFetchProgress::onFetchBegin()
{
  if (m_suppressed)
    return;
  m_stream << "Starting files downloading" << std::endl;
}

void FilesFetchProgress::onFilesReading()
{
  if (m_suppressed)
    return;
  for(std::string::size_type i = 0;i < m_prevStrLen;i++)
    m_stream << "\b";
  m_prevStrLen = 0;
  m_stream << "Reading downloaded data" << std::endl;
}

void FilesFetchProgress::onFetchIsCompleted()
{
  if (m_suppressed)
    return;
  for(std::string::size_type i = 0;i < m_prevStrLen;i++)
    m_stream << "\b";
  m_prevStrLen = 0;
  m_stream << "Downloading is completed!" << std::endl;
}

void FilesFetchProgress::onFetchStatus(unsigned char currentPartPercents,
					    unsigned char totalPercents,
					    size_t partNumber,
					    size_t partCount,
					    size_t currentPartSize,
					    const std::string& currentPartName)
{
  if (m_suppressed)
    return;
  assert(totalPercents <= 100);
  std::ostringstream ss;
  if (totalPercents)
    ss << " " << (size_t)totalPercents << "% (file " << (partNumber + 1) << " of " << partCount << ", " << currentPartSize / 1024 << "k, " << currentPartName << ")"; else
    ss << (size_t)totalPercents << "% (file " << (partNumber + 1) << " of " << partCount << ", " << currentPartSize / 1024 << "k, " << currentPartName << ")";
  for(std::string::size_type i = 0;i < m_prevStrLen;i++)
    m_stream << "\b";
  m_stream << ss.str();
  m_prevStrLen = ss.str().length();
}

DEEPSOLVER_END_NAMESPACE
