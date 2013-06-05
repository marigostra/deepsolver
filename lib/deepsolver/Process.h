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

#ifndef DEEPSOLVER_PROCESS_H
#define DEEPSOLVER_PROCESS_H

namespace Deepsolver
{
  class Process
  {
  public:
    static int runSync(const std::string& bin, const StringVector& args);
  }; //class Process;
} //namespace Deepsolver;

#endif //DEEPSOLVER_PROCESS_H;
