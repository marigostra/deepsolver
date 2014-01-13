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

#ifndef DEEPSOLVER_MINISAT_CALL_DATA_H
#define DEEPSOLVER_MINISAT_CALL_DATA_H

namespace Deepsolver
{
  namespace Sat
  {
    class MinisatCallData
    {
    public:
      /**\brief The default constructor*/
      MinisatCallData()
	: m_clauseSizes(NULL),
	  m_equation(NULL),
	  m_solution(NULL),
	  m_collisions(NULL) {}

      /**\brief The destructor*/
      virtual ~MinisatCallData()
      {
	if (m_clauseSizes != NULL)
	  delete[] m_clauseSizes;
	if (m_equation != NULL)
	  {
	    for(size_t i = 0;i < m_clauseCount;i++)
	      if (m_equation[i] != NULL)
		delete[] m_equation[i];
	    delete[] m_equation;
	  }
	if (m_solution != NULL)
	  delete[] m_solution;
	if (m_collisions != NULL)
	  delete[] m_collisions;
      }

    public:
      void setClauseSizes(size_t* clauseSizes)
      {
	m_clauseSizes = clauseSizes;
      }

      void setEquation(size_t clauseCount, int** equation)
      {
	m_clauseCount = clauseCount;
	m_equation = equation;
      }

      void setSolution(unsigned char* solution)
      {
	m_solution = solution;
      }

      void setCollisions(unsigned char* collisions)
      {
	m_collisions = collisions;
      }

    private:
      size_t* m_clauseSizes;
      size_t m_clauseCount;
      int** m_equation;
      unsigned char* m_solution;
      unsigned char* m_collisions;
    }; //class MinisatCallData;
  } //namespace Sat;
} //namespace Deepsolver;

#endif //DEEPSOLVER_MINISAT_CALL_DATA_H;
