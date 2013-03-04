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

#ifndef DEEPSOLVER_TRANSACTION_ITERATOR_H
#define DEEPSOLVER_TRANSACTION_ITERATOR_H

#include"AbstractPackageBackEnd.h"
#include"ConfigCenter.h"
#include"AbstractOperationContinueRequest.h"
#include"AbstractFetchListener.h"

class TransactionIterator
{
public:
  TransactionIterator(const ConfigCenter& conf,
		      std::auto_ptr<AbstractPackageBackEnd> backEnd,
		      const PkgVector& install,
		      const PkgVector& remove,
		      const PkgVector& upgradeFrom,
		      const PkgVector& upgradeTo,
		      const PkgVector& downgradeFrom,
		      const PkgVector& downgradeTo)
    : m_conf(conf),
      m_backEnd(backEnd),
      m_install(install),
    m_remove(remove),
      m_upgradeFrom(upgradeFrom),
      m_upgradeTo(upgradeTo),
      m_downgradeFrom(downgradeFrom),
      m_downgradeTo(downgradeTo) {}

  virtual ~TransactionIterator() {}

public:
  void fetchPackages(AbstractFetchListener& listener,
		     const AbstractOperationContinueRequest& continueRequest);

  void makeChanges();

  const AbstractPackageBackEnd& getBackEnd() const
  {
    assert(m_backEnd.get() != NULL);
    return *m_backEnd.get();
  }

  bool emptyTask() const
  {
    return m_install.empty() && m_remove.empty() && m_upgradeTo.empty() && m_downgradeTo.empty();
  }

  const PkgVector& getInstall() const
  {
    return m_install;
  }

  const PkgVector& getRemove() const
  {
    return m_remove;
  }

  const PkgVector& getUpgradeFrom() const
  {
    return m_upgradeFrom;
  }

  const PkgVector& getUpgradeTo() const
  {
    return m_upgradeTo;
  }

  const PkgVector& getDowngradeFrom() const
  {
    return m_downgradeFrom;
  }

  const PkgVector& getDowngradeTo() const
  {
    return m_downgradeTo;
  }

private:
  const ConfigCenter& m_conf;
  std::auto_ptr<AbstractPackageBackEnd> m_backEnd;
  PkgVector m_install, m_remove;
  PkgVector m_upgradeFrom, m_upgradeTo;
  PkgVector m_downgradeFrom, m_downgradeTo;
  StringVector m_filesInstall, m_namesRemove;
  StringToStringMap m_filesUpgrade, m_filesDowngrade;
}; //class TransactionIterator;

#endif //DEEPSOLVER_TRANSACTION_ITERATOR_H;
