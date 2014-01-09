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

#ifndef DEEPSOLVER_TRANSACTION_PROGRESS_H
#define DEEPSOLVER_TRANSACTION_PROGRESS_H

#include"deepsolver/OperationCore.h"

namespace Deepsolver
{
  class TransactionProgress: public AbstractTransactionListener
  {
  public:
    TransactionProgress(std::ostream& stream, bool suppress)
      : m_stream(stream),
	m_suppress(suppress) {}

    virtual ~TransactionProgress() {}

  public:
    void onPkgListProcessingBegin();
    void onPkgListProcessingEnd();

  private:
    std::ostream& m_stream;
    const bool m_suppress;
  }; //class abstractTransactionListener;
} //namespace Deepsolver;

#endif //DEEPSOLVER_TRANSACTION_PROGRESS_H;
