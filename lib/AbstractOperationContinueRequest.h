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

#ifndef DEEPSOLVER_ABSTRACT_OPERATION_CONTINUE_REQUEST_H
#define DEEPSOLVER_ABSTRACT_OPERATION_CONTINUE_REQUEST_H

/**\brief The abstract interface for continuous process interruption
 *
 * Various continuous processes (such as downloading) ask external object
 * to be sure user does not want to interrupt the task being
 * performed. This class declares the interface for objects to provide
 * such information. It is called multiple times during the work. Any
 * negative answer causes immediate process cancelling.
 *
 * \sa OperationCore TransactionIterator FilesFetch
 */
class AbstractOperationContinueRequest
{
public:
  /**\brief The default constructor*/
  AbstractOperationContinueRequest() {}

  /**\brief The destructor*/
  virtual ~AbstractOperationContinueRequest() {}

public:
  /**\brief Confirms a process should be continued or signals it must be cancelled
   *
   * Implement this method for proper interruption of continuous tasks.
   *
   * \return Non-zero means to continue operation
   */
  virtual bool onContinueOperationRequest() const = 0;
}; //class AbstractOperationContinueRequest; 

#endif //DEEPSOLVER_ABSTRACT_OPERATION_CONTINUE_REQUEST_H;
