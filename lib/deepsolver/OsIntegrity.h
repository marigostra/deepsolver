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

#ifndef DEEPSOLVER_OS_INTEGRITY_H
#define DEEPSOLVER_OS_INTEGRITY_H

#include"deepsolver/AbstractPkgBackEnd.h"

namespace Deepsolver
{
  /**\brief The class for the operating system state integrity verification
   *
   * This class simulates real or modified operating system state and
   * checks if there any integrity breaks.  There are several types of
   * possible problems: unmet requires, violated conflicts etc. The testing
   * is performed by total checking of every package proposed to be
   * installed with looking through its dependencies and ensuring that all
   * of them are properly satisfied.
   */
  class OsIntegrity
  {
  public:
    /**\brief The constructor
     *
     * \param [in] backend The reference to a package back-end to perform testing with
     */
    OsIntegrity(const AbstractPkgBackEnd& backend)
      : m_backend(backend) {}

    /**\brief The destructor*/
    virtual ~OsIntegrity() {}

  public:
    /**\brief Performs OS integrity verification
     *
     * \param [in] pkgs The set of packages representing OS state
     *
     * \return Non-zero if there are no breaks  and zero otherwise
     */
    bool verify(const PkgVector& pkgs) const;

  private:
    bool checkRequire(const NamedPkgRel& require, const PkgVector& pkgs) const;
    bool checkConflict(const NamedPkgRel& conflict, const PkgVector& pkgs) const;

  private:
    const AbstractPkgBackEnd& m_backend;
  }; //class OsIntegrity;
} //namespace Deepsolver;

#endif //DEEPSOLVER_OS_INTEGRITY_H;
