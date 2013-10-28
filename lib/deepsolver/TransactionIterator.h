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

#include"deepsolver/AbstractPkgBackEnd.h"
#include"deepsolver/ConfigCenter.h"
#include"deepsolver/AbstractContinueRequest.h"
#include"deepsolver/AbstractFetchListener.h"

namespace Deepsolver
{
  class TransactionIterator
  {
  public:
    TransactionIterator(const ConfigCenter& conf,
			std::auto_ptr<AbstractPkgBackEnd> backend,
			const PkgVector& install,
			const PkgVector& remove,
			const PkgVector& upgradeFrom,
			const PkgVector& upgradeTo,
			const PkgVector& downgradeFrom,
			const PkgVector& downgradeTo)
      : m_conf(conf),
	m_backend(backend),
	m_install(install),
	m_remove(remove),
	m_upgradeFrom(upgradeFrom),
	m_upgradeTo(upgradeTo),
	m_downgradeFrom(downgradeFrom),
	m_downgradeTo(downgradeTo) {}

    /**\brief The destructor*/
  virtual ~TransactionIterator() {}

  public:
    void getUrls(StringVector& toInstall,
		 StringVector& toUpgrade,
		 StringVector& toDowngrade) const;

    void fetchPackages(AbstractFetchListener& listener,
		       const AbstractOperationContinueRequest& continueRequest);

    void makeChanges();

    const AbstractPkgBackEnd& getBackEnd() const
    {
      assert(m_backend.get() != NULL);
      return *m_backend.get();
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

    const StringVector& getFilesInstall() const
    {
      return m_filesInstall;
    }

    const StringToStringMap& getFilesUpgrade() const
    {
      return m_filesUpgrade;
    }

    const StringToStringMap& getFilesDowngrade() const
    {
      return m_filesDowngrade;
    }

  private:
    const ConfigCenter& m_conf;
    std::auto_ptr<AbstractPkgBackEnd> m_backend;
    PkgVector m_install, m_remove;
    PkgVector m_upgradeFrom, m_upgradeTo;
    PkgVector m_downgradeFrom, m_downgradeTo;
    StringVector m_filesInstall, m_namesRemove;
    StringToStringMap m_filesUpgrade, m_filesDowngrade;
  }; //class TransactionIterator;
} //namespace Deepsolver;

#endif //DEEPSOLVER_TRANSACTION_ITERATOR_H;
