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

#ifndef DEEPSOLVER_UTILS_H
#define DEEPSOLVER_UTILS_H

namespace Deepsolver
{
  template<typename T>
class Dichotomy
  {
  public:
    typedef typename std::vector<T> Vector;
    typedef typename Vector::size_type size_type;

  public:
    /**\brief The default constructor*/
    Dichotomy() {}

    /**\brief The destructor*/
    virtual ~Dichotomy() {}

  public:
    bool findSingle(const Vector& v,
		    T value,
		    size_type& pos)
    {
      if (v.empty())
	{
	  pos = 0;
	return 0;
	}
      StringVector::size_type l = 0, r = v.size();
      while(l + 1 < r)
	{
	  const size_type c = (l + r) / 2;
	  assert(c < v.size());
	  if (v[c] == value)
	    {
	      pos = c;
	      return 1;
	    }
	  if (v[c] > value)
	    r = c; else 
	    l = c;
	}
      assert(l == r || l + 1 == r);
      assert(l < v.size());
      if (v[l] == value)
	{
	  pos = l;
	  return 1;
	}
      return 0;
    }

    bool findMultiple(const Vector& v,
		      T value,
		      size_type& fromPos,
		      size_type& toPos)
    {
      if (v.empty())
	{
	  fromPos = 0;
	  toPos = 0;
	  return 0;
	}
      size_type l = 0, r = v.size();
      while(l + 1 < r)
	{
	  assert(l < v.size() && r <= v.size());
	  const size_type center = (l + r) / 2;
	  assert(center < v.size());
	  if (v[center] == value)
	    {
	      fromPos = center;
	      toPos = center;
	      adjustBounds(v, value, fromPos, toPos);
	      return 1;
	    }
	  if (v[center] > value)
	    r = center; else
	    l = center;
	}
      assert(l == r || l + 1 == r);
      assert(l < v.size());
      if (v[l] != value)
	{
	  fromPos = 0;
	  toPos = 0;
	  return 0;
	}
      fromPos = l;
      toPos = l;
      adjustBounds(v, value, fromPos, toPos);
      return 1;
    }

  private:
    void adjustBounds(const Vector& v,
		      T value, 
		      size_type& fromPos,
		      size_type& toPos)
    {
      assert(fromPos == toPos);
      assert(fromPos < v.size());
      assert(v[fromPos] == value);
      while(fromPos > 0 && v[fromPos] == value)//size_type is unsigned, so all known overflow troubles are possible here, be careful!
	fromPos--;
      assert(fromPos < v.size());
      if (v[fromPos] != value)
	fromPos++;
      assert(fromPos < v.size());
      assert(v[fromPos] == value);
      while(toPos < v.size() && v[toPos] == value)
	toPos++;
      assert(toPos <= v.size());
      assert(fromPos <= toPos);
    }
  }; //class Dichotomy;

  template<typename T>
  void noDoubling(std::vector<T>& v)
  {
    if (v.size() < 64)
      {
	std::vector<T> vv;
	for (size_t i = 0;i < v.size();i++)
	  {
	    size_t j;
	    for(j = 0;j < i;j++)
	      if (v[i] == v[j])
		break;
	    if (j == i)
	      vv.push_back(v[i]);
	  }
	v = vv;
	return;
      }
    //Be careful: the following only for items with operator < and operator >;
    std::set<T> s;
    for(size_t i = 0;i < v.size();i++)
      s.insert(v[i]);
    v.resize(s.size());
    size_t k = 0;
    typename std::set<T>::const_iterator it;
    for(it = s.begin();it != s.end();it++)
      v[k++] = *it;
    assert(k == s.size());
  }

} //namespace Deepsolver;

#endif //DEEPSOLVER_UTILS_H;
