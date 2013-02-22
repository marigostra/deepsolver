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

#ifndef DEEPSOLVER_ALWAYS_TRUE_CONTINUE_REQUEST_H
#define DEEPSOLVER_ALWAYS_TRUE_CONTINUE_REQUEST_H

#include"AbstractOperationContinueRequest.h"

class AlwaysTrueContinueRequest: public AbstractOperationContinueRequest
{
public:
  AlwaysTrueContinueRequest() {}
  virtual ~AlwaysTrueContinueRequest() {}

public:
  bool onContinueOperationRequest() const
  {
    return 1;
  }
}; //class AlwaysTrueContinueRequest; 

#endif //DEEPSOLVER_ALWAYS_TRUE_CONTINUE_REQUEST_H;
