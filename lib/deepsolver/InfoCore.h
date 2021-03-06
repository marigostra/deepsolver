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

#ifndef DEEPSOLVER_INFO_CORE_H
#define DEEPSOLVER_INFO_CORE_H

#include"deepsolver/ConfigCenter.h"
#include"deepsolver/AutoReleaseStrings.h"

namespace Deepsolver 
{
  class InfoCore: private AutoReleaseStrings
  {
  public:
    /**\brief The constructor
     *
     * \param [in conf A complete  configuration information
     */
    InfoCore(const ConfigCenter& conf): 
      m_conf(conf)  {}

    /**\brief The destructor*/
    virtual ~InfoCore() {}

  public:
    void listKnownPackages(PkgVector& pkgs, bool noInstalled, bool noRepoAvailable);

  private:
    const ConfigCenter& m_conf;
  }; //class InfoCore;
} //namespace Deepsolver;

#endif //DEEPSOLVER_INFO_CORE_H;
