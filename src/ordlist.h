#ifndef __ordlist_h
#define __ordlist_h
/*
   ordlist.h : This file is part of pstoedit 
   simple template for a sorted list. I did not want to use STL
   because not all compilers support it yet. 
  
   Copyright (C) 1993 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
#ifndef cppcomp_h
#include "cppcomp.h"
#endif
#ifndef assert
#include <assert.h>
#endif
#include I_stdlib
#include I_iostream

//lint -sem(ordlist*::insert,custodial(1))
//
// Telem is the type under which an element is stored, so either T  or T &
//
template <class T,class Telem,class COMPARATOR>
class ordlist {
private:
	class ordlistElement {
	public:
		ordlistElement(const T& e,ordlistElement * n): next(n),elem(e) {}
		ordlistElement* next;
		Telem elem;
	};
public:
	ordlist(): 
		first(0),
		l_size(0),
		lastaccessptr((new ordlistElement *)),
		lastaccessindexptr((new size_t)) {}
		// initialize the Refs with objects on the heap, because we need
		// to modify these from within const functions
		// (these act as a sort of cache, but they do not influence constness)
	~ordlist() {
		clear();
		delete (lastaccessptr); lastaccessptr=0;
		delete (lastaccessindexptr);lastaccessindexptr=0;
		first = 0;
	}
	void clear() {
		ordlistElement * cur = first;
		while(cur != 0) {  ordlistElement * next = cur-> next; delete cur; cur = next; }
		l_size = 0;
		first = 0;
		(*lastaccessptr) = 0;
		(*lastaccessindexptr) = 0;
	}
	void insert(const T& elem) {
		if (first == 0) {
			first = new ordlistElement(elem,0);
		} else {
			if (COMPARATOR::compare(first->elem , elem) ) {
				ordlistElement * newelem = new ordlistElement(elem,first);
				first = newelem;
			} else {
				ordlistElement * next    = first->next;
				ordlistElement * current = first;
				while (current != 0) {
					if ( (next == 0) || COMPARATOR::compare(next->elem , elem) ) {
						ordlistElement * newelem = new ordlistElement(elem,next);
						current->next = newelem;
						break;
					} 
					current = next;
					next = next->next;
				}
			}
		}
		l_size++;
		*lastaccessptr = first;
		*lastaccessindexptr = 0;
	}
	const T &operator[](size_t i) const {
		ordlistElement * start = 0;
		if (i < size() ) {
			size_t ind = 0;
			if (i == (*lastaccessindexptr) ) { 
//				cerr << "returning from last " << endl;
				return (*lastaccessptr)->elem;
			} else if (i < (*lastaccessindexptr) ) { 
//				cerr << "returning via search from start " << endl;
				start = first;
				ind = 0;
			} else {
//				cerr << "returning via forward search" << endl;
				start = (*lastaccessptr);
				ind = (*lastaccessindexptr);
			}
			assert(start);
			while(ind  < i) { start = start->next; ind++;}
			// we need to cast away const for caching
			(*lastaccessptr) = start;
			(*lastaccessindexptr) = i;
//			((ordlist<T,Telem,COMPARATOR> *)this)->lastaccess = start;
//			((ordlist<T,Telem,COMPARATOR> *)this)->lastaccessindex = i;
			return start->elem;
		} else {
			cerr << "illegal index " << i << " max : " << size() << endl;
			assert(i < size());
                        static T nullElement;
                        return nullElement; // should not be reached anyway
		}
	}
	size_t size() const { return l_size;}

#ifdef TEST
	void dump() const {
		int i = 0;
		ordlistElement * cur = first;
		while(cur != 0) { cout << '[' << i << "] :" << cur->elem << endl;  i++; cur = cur->next; }
	}
#endif

private:
	ordlistElement * first;
	size_t l_size ;

	// helpers for faster sequential access via operator[]
	// these remember the position of the last access
	ordlistElement **lastaccessptr;
	size_t * lastaccessindexptr ;

	// inhibitors: (may not be called)
	ordlist(const ordlist<T,Telem,COMPARATOR> &);
	const ordlist<T,Telem,COMPARATOR> & operator=(const ordlist<T,Telem,COMPARATOR> &);
};


#ifdef TEST
template <class T>
class GreaterThan {
public:
	static bool compare(const T & o1, const T & o2) { return o1 > o2 ;}
};

template <class T>
class LessThan {
public:
	static bool compare(const T & o1, const T & o2) { return o1 < o2 ;}
};

int main()
{
	{
	ordlist<int,int,LessThan<int> > oli;
	oli.insert(5);
	oli.insert(7);
	oli.insert(1);
	oli.insert(2);
	oli.insert(9);
	oli.insert(10);
	oli.insert(41);
	oli.insert(0);
	cout << oli.size() << endl;
	cout << oli[0] << endl;
	cout << " begin of Dump " << endl;
	oli.dump();
	cout << " end of Dump " << endl;
	for (unsigned int i = 0; i < oli.size(); i++ ) {
		cout << oli[i] << " "<< endl;
	}
	cout << oli[3] << endl;
	}

	{
	ordlist<int,const int&,LessThan<int> > oli;
	cout << " begin of Dump " << endl;
	oli.insert(1);
	oli.dump();
	cout << " end of Dump " << endl;
	for (unsigned int i = 0; i < oli.size(); i++ ) {
		cout << oli[i] << " "<< endl;
	}
	}
	{
	ordlist<int,int,LessThan<int> > oli;
	cout << " begin of Dump " << endl;
	oli.dump();
	cout << " end of Dump " << endl;
	for (unsigned int i = 0; i < oli.size(); i++ ) {
		assert(oli.size() == 0);
	}
	// test for assertion 
	// cout << oli[3] << endl;
	}

#if 0 
// should not compile due to inhibitors
	{
		ordlist<float,float,LessThan<int> > ol1;
		ordlist<float,float,LessThan<int> > ol2;
		ol1 = ol2;
		ordlist<float,float,LessThan<int> > ol3 = ol1;
	}
#endif

	return 0;
}
#endif


// include guard
#endif
 
 
