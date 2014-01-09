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

#ifndef DEEPSOLVER_ABSTRACT_TRANSACTION_LISTENER_H
#define DEEPSOLVER_ABSTRACT_TRANSACTION_LISTENER_H

namespace Deepsolver
{
  /**\brief The interface to follow transaction progress
   *
   * With this class client application can follow the transaction process
   * and show corresponding notifications to a user. This interface is used
   * for transactions of any kind, including installation, removing,
   * fetching-only transactions etc.
   *
   * \sa OperationCore OperationCoreException
   */
  class AbstractTransactionListener
  {
  public:
    /**\brief The default constructor*/
    AbstractTransactionListener() {}

    /**\brief The destructor*/
    virtual ~AbstractTransactionListener() {}

  public:
    /**\brief Notifies the operation core begins building the lists of packages for installation and removing*/
    virtual void onPkgListProcessingBegin() = 0;

    /**\brief Notifies the operation core finishes building the lists of packages for installation and removing*/
    virtual void onPkgListProcessingEnd() = 0;
  }; //class abstractTransactionListener;
} //namespace Deepsolver;

#endif //DEEPSOLVER_ABSTRACT_TRANSACTION_LISTENER_H;
