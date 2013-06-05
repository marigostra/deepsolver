
#include<assert.h>
#include<iostream>
#include<vector>
#include<string>
#include"RpmTransaction.h"
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>

static void RPMSTOP(const std::string& fnName)
{
  std::cerr << "rpmtrans:" << fnName << " failed" << std::endl;
  exit(EXIT_FAILURE);
}

void RpmTransaction::init()
{
  if (m_initialized)
    return;
  rpmReadConfigFiles(NULL, NULL);
  if (rpmdbOpen("", &m_db, O_RDWR, 0644) != 0)//FIXME:root directory;
    RPMSTOP("rpmdbOpen()");
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
			     const StringVector& upgrade)
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
  rc = rpmRunTransactions(m_ts, rpmShowProgress, (void *)notifyFlags, NULL,
			  &probs, (rpmtransFlags)tsFlags,
			  (rpmprobFilterFlags)probFilter);
  if (rc > 0) 
    {
      std::cerr << "Error while running transaction" << std::endl;
      //	if (probs->numProblems > 0)
      //	  rpmpsPrint(stderr, probs);
    } else 
    {
      if (rc < 0)
	std::cerr << "warning:some errors occurred while running transaction" << std::endl; 
    }
  std::cout << "Done." << std::endl;


  //  rpmpsFree(probs);

}

void RpmTransaction::addToTransactionInstall(const StringVector& files)
{
  for (StringVector::size_type i = 0;i < files.size();i++)
    {
      FD_t fd = Fopen(files[i].c_str(), "r.ufdio");
      if (fd == NULL)
	RPMSTOP("Fopen()");
      Header hdr;
      int rc = rpmReadPackageHeader(fd, &hdr, 0, NULL, NULL);
      if (rc != 0)
	RPMSTOP("rpmReadPackageHeader()");
      rc = rpmtransAddPackage(m_ts, hdr, NULL, files[i].c_str(), 0, 0);
      if (rc != 0)
	RPMSTOP("rpmtransAddPackage()");
      headerFree(hdr);
      Fclose(fd);
    }
}

void RpmTransaction::addToTransactionUpgrade(const StringVector& files)
{
  for (StringVector::size_type i = 0;i < files.size();i++)
    {
      FD_t fd = Fopen(files[i].c_str(), "r.ufdio");
      if (fd == NULL)
	RPMSTOP("Fopen()");
      Header hdr;
      int rc = rpmReadPackageHeader(fd, &hdr, 0, NULL, NULL);
      if (rc != 0)
	RPMSTOP("rpmReadPackageHeader()");
      rc = rpmtransAddPackage(m_ts, hdr, NULL, files[i].c_str(), 1, 0);
      if (rc != 0)
	RPMSTOP("rpmtransAddPackage()");
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
  RPMSTOP("rpmtransRemovePackage()");
	    }
	}
      MI = rpmdbFreeIterator(MI);
    }
}

int main()
{
  StringVector install, remove, upgrade;
  install.push_back("lopsus-page-1.4.0.1-alt1.i586.rpm");
  RpmTransaction trans;
  trans.init();
  trans.process(install, remove, upgrade);
  return 0;
}
