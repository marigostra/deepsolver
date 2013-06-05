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

#ifndef DEEPSOLVEr_PACKAGE_LIST_PRINTING_H
#define PAcKAGE_LIST_PRINTING_H

#include"deepsolver/OperationCore.h"

namespace Deepsolver
{
  class PackageListPrinting
  {
  public:
    PackageListPrinting(const ConfigCenter& conf)
      : m_conf(conf) {}

    virtual ~PackageListPrinting() {}

  public:
    void printSolution(const TransactionIterator& it) const;

  private:
    void columnsView(const StringVector& install,
		     const StringVector& remove,
		     const StringVector& upgrade,
		     const StringVector& downgrade) const;

  private:
    const ConfigCenter& m_conf;
  }; //class PackageListPrinting;
} //namespace Deepsolver;

#endif //PAcKAGE_LIST_PRINTING_H;
