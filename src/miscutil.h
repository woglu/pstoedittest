#ifndef miscutil_h
#define miscutil_h
/*
   miscutil.h : This file is part of pstoedit
   header declaring misc utility functions

   Copyright (C) 1998 - 2024 Wolfgang Glunz, wglunz35_AT_pstoedit.net

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

#include I_iostream
#include I_ostream
#include I_istream
#include I_fstream
#include I_string_h

#ifdef OS_WIN32_WCE
#include "netwdm.h"
#endif
//lint -efile(451,math.h)
#include <math.h>

USESTD

#ifndef assert
#include <assert.h>
#endif


#if defined(_WIN32) || defined(__OS2__)
constexpr char directoryDelimiter = '\\';
#else
constexpr char directoryDelimiter = '/';
#endif



// NOTE: The following two dup functions are made inline to solve the problem
// of allocation and deallocation in different .dlls. 
// a strdup which uses new instead of malloc

inline char * cppstrndup(const char * const src, const size_t length, const size_t addon = 0 )
{
	assert(src);
	const size_t lp1 = length+1; 
	auto const ret = new char[lp1 + addon];
	for (size_t i = 0 ; i < lp1; i++)
	{
		ret[i] = src[i];
	}
	ret[length] = '\0';
	return ret;

}
inline char *cppstrdup(const char * const src, const size_t addon = 0)
{
	return cppstrndup(src,strlen(src),addon);
}

DLLEXPORT unsigned short hextoint(const char hexchar) ;

// A temporary file, that is automatically removed after usage
class  TempFile  {
public:
	DLLEXPORT TempFile()  ;
	DLLEXPORT ~TempFile() ;
	DLLEXPORT ofstream & asOutput();
	DLLEXPORT ifstream & asInput();
private:
	void close() ;
	char * tempFileName;
	ofstream outFileStream;
	ifstream inFileStream;

	NOCOPYANDASSIGN(TempFile)
};


#if defined (_MSC_VER) 
#pragma warning(disable: 4275)
#pragma warning(disable: 4251)
// non dll-interface class 'Mapper<class KeyValuePair<class RSString,class RSString> >' used as base for dll-interface class 'FontMapper'
// miscutil.h(259) : see declaration of 'FontMapper'
// Same applies to string
#endif

//#ifdef NOTYETFULLYBASEDONSTL
#include <string>
typedef std::string RSString;
inline bool string_contains(const RSString & s, const RSString & substr) { return s.find(substr) != RSString::npos; }
#define DONOTIMPLEMENTRSSTRING
// FIXME


inline bool strequal(const char * const s1, const char * const s2) { return (strcmp(s1,s2) == 0);}

class Argv {
	static constexpr unsigned int maxargs = 1000;
public:
	unsigned int argc = 0;
	char* argv[maxargs] = { nullptr };
	Argv() = default;
	~Argv() { clear(); }

	void addarg(const char * const arg) { 
		assert(argc<maxargs); //lint !e1776
		if (argc < maxargs) {
		   argv[argc] = cppstrdup(arg); 
		   argc++; 
		}
	}
	void addarg(const RSString & arg) { 
		addarg(arg.c_str());
	}

	unsigned int parseFromString(const char * const argstring);
	
	void clear() {
		for (unsigned int i = 0; (i < argc) &&  (i < maxargs); i++) {
			delete [] argv[i] ; argv[i]= nullptr; 
		}
		argc = 0;
	}
	
	NOCOPYANDASSIGN(Argv)
};
DLLEXPORT ostream & operator <<(ostream & out, const Argv & a);


DLLEXPORT bool fileExists (const char * filename);
DLLEXPORT RSString full_qualified_tempnam(const char * pref);
DLLEXPORT void convertBackSlashes(char* fileName);


template <class T> 
class DLLEXPORT Mapper {
public:
	Mapper() : firstEntry(nullptr) {};
	virtual ~Mapper() {
		while (firstEntry != nullptr) {
			T * nextEntry = firstEntry->nextEntry;
			delete firstEntry;
			firstEntry=nextEntry;
		}
	}
public:
	void insert(const typename T::K_Type & key, const  typename T::V_Type& value) {
		firstEntry = new T(key,value,firstEntry);
	}
	const  typename T::V_Type* getValue(const  typename T::K_Type & key) const {
		T * curEntry = firstEntry;
		while (curEntry != 0) {
			if (curEntry->key() == key ) {
				return &curEntry->value();
			}
			curEntry=curEntry->nextEntry;
		}
		return 0;
	}
	T * firstEntry;

private: 
	Mapper(const Mapper<T>&) = delete;
	const Mapper<T> & operator=(const Mapper<T>&) = delete;
};

//lint -esym(1712,KeyValuePair) // no default ctor
template <class K, class V> 
class DLLEXPORT KeyValuePair
{
public:
		typedef K K_Type;
		typedef V V_Type;
		KeyValuePair(const K_Type & k,const V_Type & v, KeyValuePair<K,V> * nextE = 0):
			key_(k), value_(v), nextEntry(nextE) {}
		const K_Type & key()	const { return key_;}
		const V_Type & value()	const { return value_;}
private:
		K_Type key_;
		V_Type value_;
public:
		KeyValuePair<K,V> * nextEntry;

private: 
	KeyValuePair(const KeyValuePair<K, V>&) = delete;
	const KeyValuePair<K, V> & operator=(const KeyValuePair<K, V>&) = delete;
};


typedef KeyValuePair<RSString,RSString> FontMapping ;



//lint -esym(1790,Mapper*)
//#ifndef BUGGYGPP
class DLLEXPORT FontMapper: public Mapper<FontMapping>
//#else /* BUGGYGPP */
//class DLLEXPORT FontMapper: public Mapper<FontMapping,RSString,RSString>
//#endif /* BUGGYGPP */
{
public:
  void readMappingTable(ostream & errstream,const char * filename);
  const char * mapFont(const RSString & fontname) const;
};
#if defined (_MSC_VER) 
#pragma warning(default: 4275)
#endif



DLLEXPORT RSString getRegistryValue(ostream& errstream, const char * typekey, const char * key);
DLLEXPORT unsigned long P_GetPathToMyself(const char *name, char * returnbuffer, unsigned long buflen);
DLLEXPORT size_t searchinpath(const char* EnvPath,const char* name, char *returnbuffer,unsigned long buflen);
DLLEXPORT void errorMessage(const char * errortext); // display an error message (cerr or msgbox)
DLLEXPORT void copy_file(istream& infile,ostream& outfile) ;
DLLEXPORT RSString getOutputFileNameFromPageNumber(const char * const outputFileTemplate, const RSString & pagenumberformatOption, unsigned int pagenumber);

inline float pythagoras(const float x, const float y) { 
	return static_cast<float>(sqrt( x*x + y*y)); 
}


#endif
 
 
 
 
 
