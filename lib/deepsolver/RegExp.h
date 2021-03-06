/*
   Copyright 2011-2014 ALT Linux
   Copyright 2011-2014 Michael Pozhidaev

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

#ifndef DEEPSOLVER_REG_EXP_H
#define DEEPSOLVER_REG_EXP_H 

namespace Deepsolver
{
  class RegExp
  {
  public:
    /**\brief The default constructor*/
    RegExp()
      : m_re(NULL) {}

    /**\brief The destructor*/
  virtual ~RegExp() {close();}

  public:
    void compile(const std::string& exp);
    bool match(const std::string& line) const;
    void close();

  private:
    regex_t* m_re;
  }; //class RegExp;

  typedef std::vector<RegExp> RegExpVector;
  typedef std::list<RegExp> RegExpList;
} //namespace Deepsolver;

#endif //DEEPSOLVER_REG_EXP_H;
