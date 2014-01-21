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

#ifndef DEEPSOLVEr_PKG_LIST_PRINTING_H
#define PKG_LIST_PRINTING_H

#include"deepsolver/ConfigCenter.h"
#include"deepsolver/TransactionIterator.h"

namespace Deepsolver
{
  class PkgListPrinting
  {
  public:
    PkgListPrinting(const ConfigCenter& conf)
      : m_conf(conf) {}

  public:
    void printSolution(const TransactionIterator& it, bool toLog = 0) const;

  private:
    void columnsView(const StringVector& install,
		     const StringVector& remove,
		     const StringVector& upgrade,
		     const StringVector& downgrade) const;

  private:
    const ConfigCenter& m_conf;
  }; //class PkgListPrinting;
} //namespace Deepsolver;

#endif //PKG_LIST_PRINTING_H;
