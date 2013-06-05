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
#include"deepsolver/PostponedRequire.h"

DEEPSOLVER_BEGIN_NAMESPACE

void PostponedRequire::takeNewRequire(VarId varId,
				      const VarIdVector& alternatives, 
				      const VarIdVector& installed)
{
  assert(varId != BadVarId);
  assert(!alternatives.empty());
  assert(!installed.empty());
  m_requires.push_back(Require(installed, varId, alternatives));
}

void PostponedRequire::varIdAffected(VarId varId,
				     AbstractTaskTranslatorOutput& output,
				     VarIdVector& pending)
{
  assert(varId != BadVarId);
  for(RequireVector::size_type i = 0;i < m_requires.size();i++)
    {
      Require& r = m_requires[i];
      assert(!r.installed.empty());
      VarIdVector::size_type k;
      for(k = 0;k < r.installed.size();k++)
	if (r.installed[k] == varId)
	  break;
      if (k >= r.installed.size())
	continue;
      r.installed[k] = r.installed.back();
      r.installed.pop_back();
      if (r.installed.empty())
	{
	  output.requiredOneAnyForPackage(r.requiredBy, r.alternatives);
	  pending.push_back(r.requiredBy);//This strongly must be for package considered to remove;
	  for(VarIdVector::size_type j = 0;j < r.alternatives.size();j++)
	    pending.push_back(r.alternatives[j]);
	}
    }
  for(RequireVector::size_type i = 0;i < m_requires.size();i++)
    while(i < m_requires.size() && m_requires[i].installed.empty())
      {
	m_requires[i] = m_requires.back();
	m_requires.pop_back();
      }
}

DEEPSOLVER_END_NAMESPACE

