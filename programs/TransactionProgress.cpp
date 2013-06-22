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
#include"TransactionProgress.h"

DEEPSOLVER_BEGIN_NAMESPACE

void TransactionProgress::onPkgListProcessingBegin()
{
  if (m_suppress)
    return;
  m_stream << "Loading list of available packages...";
}

void TransactionProgress::onPkgListProcessingEnd()
{
  if (m_suppress)
    return;
  m_stream << " Done." << std::endl;
}

DEEPSOLVER_END_NAMESPACE
