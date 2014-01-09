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

#ifndef DEEPSOLVER_AUTO_RELEASE_STRINGS_H
#define DEEPSOLVER_AUTO_RELEASE_STRINGS_H

namespace Deepsolver
{
  class AutoReleaseStrings
  {
  public:
    /**\brief The default constructor*/
    AutoReleaseStrings() {}

    /**\brief The destructor*/
    virtual ~AutoReleaseStrings() 
      {
	freeAutoReleaseStrings();
      }

  public:
    size_t getAutoReleaseStringCount() const
    {
      return m_autoReleaseStrings.size();
    } 

    const char* getAutoReleaseString(size_t index)
    {
      assert(index < m_autoReleaseStrings.size());
      return m_autoReleaseStrings[index];
    }

    void addAutoReleaseString(const char* value)
    {
      assert(value);
      m_autoReleaseStrings.push_back(value);
    }

    void freeAutoReleaseStrings()
    {
      for(ConstCharVector::size_type i = 0;i < m_autoReleaseStrings.size();i++)
	delete[] m_autoReleaseStrings[i];
      m_autoReleaseStrings.clear();
    }

  protected:
    ConstCharVector m_autoReleaseStrings;
  }; //class AutoReleaseStrings;
} //namespace Deepsolver;

#endif //DEEPSOLVER_AUTO_RELEASE_STRINGS_H;
