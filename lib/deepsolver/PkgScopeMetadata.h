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

#ifndef DEEPSOLVER_PKG_SCOPE_BMETADATA_H
#define DEEPSOLVER_PKG_SCOPE_METADATA_H

#include"deepsolver/PkgScopeBase.h"

namespace Deepsolver
{
  class PkgScopeMetadata: public PkgScopeBase
  {
  public:
    PkgScopeMetadata(const AbstractPkgBackEnd& backend, const Snapshot& snapshot)
      : PkgScopeBase(backend, snapshot) {}

    /**\brief The destructor*/
  virtual ~PkgScopeMetadata() {}

  public:
  void initMetadata();

  protected:
    void findProviders(PkgId providePkgId, VarIdVector& res) const;
    void findPkgsByRequire(PkgId requirePkgId, VarIdVector& res) const;
    void findPkgsByConflict(PkgId conflictPkgId, VarIdVector& res) const;

  private:
    void fillRevMapProvides();
    void fillRevMapRequires();
    void fillRevMapConflicts();

  private:
  template<typename T1, typename T2>
    struct DichotomyItem
    {
    DichotomyItem(T1 f, T2 s)
    : first(f),
	second(s) {}

      bool operator ==(const DichotomyItem<T1, T2>& d) const {return first == d.first;};
      bool operator !=(const DichotomyItem<T1, T2>& d) const {return first != d.first;};
      bool operator <(const DichotomyItem<T1, T2>& d) const {return first < d.first;};
      bool operator <=(const DichotomyItem<T1, T2>& d) const {return first <= d.first;};
      bool operator >(const DichotomyItem<T1, T2>& d) const {return first > d.first;};
      bool operator >=(const DichotomyItem<T1, T2>& d) const {return first >= d.first;};

      T1 first;
      T2 second;
    }; //struct DichotomyItem; 

  private:
  typedef DichotomyItem<PkgId, VarId> RevMapItem;
  typedef std::vector<RevMapItem> RevMap;

  private:
  RevMap m_revMapProvides, m_revMapInstalledRequires, m_revMapInstalledConflicts;
  }; //class PkgScopeBase;
} //namespace Deepsolver;

#endif //DEEPSOLVER_Pkg_SCOPE_BASE_H;
