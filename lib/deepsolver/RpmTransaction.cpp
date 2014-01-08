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
#include"deepsolver/RpmTransaction.h"
#include"rpm/rpmcli.h"

DEEPSOLVER_BEGIN_NAMESPACE

void RpmTransaction::init()
{
  if (m_initialized)
    return;
  rpmReadConfigFiles(NULL, NULL);
  if (rpmdbOpen("", &m_db, O_RDWR, 0644) != 0)//FIXME:root directory;
    throw PkgBackEndException("rpmdbOpen()");
  m_ts = rpmtransCreateSet(m_db, "");//FIXME:root directory;
  m_initialized = 1;
}

void RpmTransaction::close()
{
  if (!m_initialized)
    return;
  rpmdbClose(m_db);
  m_initialized = 0;
}

void RpmTransaction::process(const StringVector& install,
			     const StringVector& remove,
			     const StringToStringMap& upgrade,
			     const StringToStringMap& downgrade)
{
  int rc = 0;
  int probFilter = 0;
  int notifyFlags = 0;
  int tsFlags = 0;
  rpmProblemSet probs;
  if (!upgrade.empty()) 
    probFilter |= RPMPROB_FILTER_OLDPACKAGE;
  //  probFilter |= RPMPROB_FILTER_REPLACEPKG//reinstall;;
  //  probFilter |= RPMPROB_FILTER_REPLACEOLDFILES;
  //      probFilter |= RPMPROB_FILTER_REPLACENEWFILES;
  notifyFlags |= INSTALL_LABEL | INSTALL_HASH;
  if (!remove.empty())
    addToTransactionRemove(remove);
  if (!install.empty())
    addToTransactionInstall(install);
  if (!upgrade.empty())
    addToTransactionUpgrade(upgrade);
  rpmDependencyConflict conflicts = NULL;
  int numConflicts;
  if (rpmdepCheck(m_ts, &conflicts, &numConflicts) || conflicts) 
    {
      std::cerr << "Conflicts found!!" << std::endl;
      if (conflicts)
	{
	  printDepProblems(stderr, conflicts, numConflicts);
	  rpmdepFreeConflicts(conflicts, numConflicts);
	}
      exit(EXIT_FAILURE);
    }
  rc = rpmdepOrder(m_ts);
  if (rc > 0) 
    {
      std::cerr << "Ordering failed" << std::endl;
      return;
    }
  rc = rpmRunTransactions(m_ts, rpmShowProgress, (void *)(intptr_t)notifyFlags, NULL,
			  &probs, (rpmtransFlags)tsFlags,
			  (rpmprobFilterFlags)probFilter);
  if (rc > 0) 
    {
      std::cerr << "Error while running transaction" << std::endl;
      //	if (probs->numProblems > 0)
      //	  rpmpsPrint(stderr, probs);
    } else 
    if (rc < 0)
      std::cerr << "warning:some errors occurred while running transaction" << std::endl; 
  //  rpmpsFree(probs);
}

void RpmTransaction::addToTransactionInstall(const StringVector& files)
{
  for (StringVector::size_type i = 0;i < files.size();i++)
    {
      FD_t fd = Fopen(files[i].c_str(), "r.ufdio");
      if (fd == NULL)
	throw PkgBackEndException("Fopen(" + files[i] + ")");
      Header hdr;
      int rc = rpmReadPackageHeader(fd, &hdr, 0, NULL, NULL);
      if (rc != 0)
	throw PkgBackEndException("rpmReadPackageHeader()");
      rc = rpmtransAddPackage(m_ts, hdr, NULL, files[i].c_str(), 0, 0);
      if (rc != 0)
	throw PkgBackEndException("rpmtransAddPackage()");
      headerFree(hdr);
      Fclose(fd);
    }
}

void RpmTransaction::addToTransactionUpgrade(const StringToStringMap& files)
{
  for (StringToStringMap::const_iterator it = files.begin();it != files.end();it++)
    {
      FD_t fd = Fopen(it->second.c_str(), "r.ufdio");
      if (fd == NULL)
	throw PkgBackEndException("Fopen(" + it->second + ")");
      Header hdr;
      int rc = rpmReadPackageHeader(fd, &hdr, 0, NULL, NULL);
      if (rc != 0)
	throw PkgBackEndException("rpmReadPackageHeader()");
      rc = rpmtransAddPackage(m_ts, hdr, NULL, it->second.c_str(), 1, 0);
      if (rc != 0)
	throw PkgBackEndException("rpmtransAddPackage()");
      headerFree(hdr);
      Fclose(fd);
    }
}

void RpmTransaction::addToTransactionRemove(const StringVector& files)
{
  for (StringVector::size_type i = 0;i < files.size();i++)
    {
      rpmdbMatchIterator MI;
      MI = rpmdbInitIterator(m_db, RPMDBI_LABEL, files[i].c_str(), 0);
      Header hdr;
      while ((hdr = rpmdbNextIterator(MI)) != NULL) 
	{
	  const unsigned int recOffset = rpmdbGetIteratorOffset(MI);
	  if (recOffset) 
	    {
	      const int rc = rpmtransRemovePackage(m_ts, recOffset);
if (rc != 0)
  throw PkgBackEndException("rpmtransRemovePackage()");
	    }
	}
      MI = rpmdbFreeIterator(MI);
    }
}

DEEPSOLVER_END_NAMESPACE


