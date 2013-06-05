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

#ifndef DEEPSOLVER_INLINE_IO_H
#define DEEPSOLVER_INLINE_IO_H

namespace Deepsolver
{
  inline size_t readSizeValue(std::ifstream& s)
  {
    size_t value = 0;
    s.read((char*)&value, sizeof(size_t));
    assert(s);//FIXME:must be an exception;
    return value;
  }

  inline unsigned short readUnsignedShortValue(std::ifstream& s)
  {
    unsigned short value = 0;
    s.read((char*)&value, sizeof(unsigned short));
    assert(s);//FIXME:must be an exception;
    return value;
  }

  inline char readCharValue(std::ifstream& s)
  {
    char value = 0;
    s.read(&value, sizeof(char));
    assert(s);//FIXME:must be an exception;
    return value;
  }

  static void   readBuf(std::ifstream& s, char* buf, size_t bufSize)
  {
    assert(buf != NULL);
    size_t count = 0;
    while(count < bufSize)
      {
	const size_t toRead = (bufSize - count) > DEEPSOLVER_IO_BLOCK_SIZE?DEEPSOLVER_IO_BLOCK_SIZE:(bufSize - count);
	s.read(buf + count, toRead);
	assert(s);//FIXME:must be an exception;
	count += toRead;
      }
    assert(count == bufSize);
  }

  inline void writeSizeValue(std::ofstream& s, size_t value)
  {
    s.write((char*)&value, sizeof(size_t));
  }

  inline void writeUnsignedShortValue(std::ofstream& s, unsigned short value)
  {
    s.write((char*)&value, sizeof(unsigned short));
  }

  inline void writeCharValue(std::ofstream& s, char value)
  {
    s.write(&value, sizeof(char));
  }

  inline void writeStringValue(std::ofstream& s, const char* value)
  {
    assert(value != NULL);
    s.write(value, strlen(value) + 1);
  }
} //namespace Deepsolver;

#endif //DEEPSOLVER_INLINE_IO_H;
