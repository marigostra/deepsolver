

#ifndef DEEPSOLVER_RPM_TRANSACTION_H
#define DEEPSOLVER_RPM_TRANSACTION_H

#include<rpm/rpmlib.h>
#include<rpm/rpmcli.h>

typedef std::vector<std::string> StringVector;

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
	       const StringVector& upgrade);

private:
  void addToTransactionInstall(const StringVector& files);
  void addToTransactionUpgrade(const StringVector& files);
  void addToTransactionRemove(const StringVector& files);

private:
  bool m_initialized;
  rpmdb m_db;
  rpmTransactionSet m_ts;
}; //class RpmTransaction;

#endif //DEEPSOLVER_RPM_TRANSACTION_H;
