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

#ifndef DEEPSOLVER_POSTPONED_REQUIRE_H
#define DEEPSOLVER_POSTPONED_REQUIRE_H

#include"deepsolver/AbstractTaskTranslatorOutput.h"

namespace Deepsolver
{
  class PostponedRequire
  {
  public:
    /**\brief The default constructor*/
    PostponedRequire() {}

    /**\brief The destructor*/
    virtual ~PostponedRequire() {}

  public:
    void takeNewRequire(VarId varId,
			const VarIdVector& alternatives, 
			const VarIdVector& installed);

    void varIdAffected(VarId varId,
		       AbstractTaskTranslatorOutput& output,
		       VarIdVector& pending);

  private:
    struct Require
    {
      Require(const VarIdVector& i,
	      VarId r,
	      const VarIdVector& a)
	: installed(i),
	  requiredBy(r),
	  alternatives(a) {}

      VarIdVector installed;
      VarId requiredBy;
      VarIdVector alternatives;
    }; //struct Require;

  private:
    typedef std::vector<Require> RequireVector;
    typedef std::list<Require> RequireList;

  private:
    RequireVector m_requires;
  }; //class PostponedRequire;
} //namespace Deepsolver;

#endif //DEEPSOLVER_POSTPONED_REQUIRE_H;
