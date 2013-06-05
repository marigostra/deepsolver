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

#ifndef DEEPSOLVER_RPM_TRANSACTION_H
#define DEEPSOLVER_RPM_TRANSACTION_H

#include<rpm/rpmlib.h>

namespace Deepsolver
{
  class RpmTransaction
  {
  public:
    RpmTransaction()
      : m_initialized(0) {}

    virtual ~RpmTransaction() {close();}

  public:
    void init();
    void close();
    void process(const StringVector& install,
		 const StringVector& remove,
		 const StringToStringMap& upgrade,
		 const StringToStringMap& downgrade);

  private:
    void addToTransactionInstall(const StringVector& files);
    void addToTransactionUpgrade(const StringToStringMap& files);
    void addToTransactionRemove(const StringVector& files);

  private:
    bool m_initialized;
    rpmdb m_db;
    rpmTransactionSet m_ts;
  }; //class RpmTransaction;

} //namespace Deepsolver;

#endif //DEEPSOLVER_RPM_TRANSACTION_H;
