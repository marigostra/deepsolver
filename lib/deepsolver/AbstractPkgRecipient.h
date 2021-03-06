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

#ifndef DEEPSOLVER_ABSTRACT_PKG_RECIPIENT_H
#define DEEPSOLVER_ABSTRACT_PKG_RECIPIENT_H

namespace Deepsolver 
{
  /**\brief The interface for handling sequences of package records
   *
   * This abstract class declares unified interface for any object purposed
   * for accepting series of package entries. Basically it was created for
   * various package information loading tasks.
   *
   * \sa Repository PackageScopeContentBuilder
   */
  class AbstractPkgRecipient
  {
  public:
    /**\brief The default constructor*/
    AbstractPkgRecipient() {}

    /**\brief The destructor*/
    virtual ~AbstractPkgRecipient() {}

  public:
    /**\brief Accepts new package file entry
     *
     * This method is called each time new package file information is available.
     *
     * \param [in] pkgFile The newly available package file entry
     */
    virtual void onNewPkgFile(const PkgFile& pkgFile) = 0;
  }; //class AbstractPkgRecipient;
} //namespace Deepsolver;

#endif //DEEPSOLVER_ABSTRACT_PKG_RECIPIENT_H;
