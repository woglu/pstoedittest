#ifndef _POPTIONS_H
#define _POPTIONS_H
/*
   poptions.h : This file is part of pstoedit
   program option handling 

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

#include I_iostream
#include I_ostream
#include I_istream

#include "miscutil.h"
#include <vector>

USESTD

enum baseTypeIDs { int_ty, double_ty, bool_ty, char_ty };

class DLLEXPORT IntValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, int &result);
	static const char *gettypename() ;
	static unsigned int gettypeID();
};

class DLLEXPORT DoubleValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, double &result) ;
	static const char *gettypename() ;
	static unsigned int gettypeID();
};


class DLLEXPORT CharacterValueExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, char &result) ;
	static const char *gettypename() ;
	static unsigned int gettypeID();
};

class DLLEXPORT BoolBaseExtractor {
public:
	static const char *gettypename();
	static unsigned int gettypeID();
};

//lint -esym(1790,BoolBaseExtractor)
#if 0
class DLLEXPORT BoolInvertingExtractor : public BoolBaseExtractor {
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, bool &result) ;
};

class DLLEXPORT BoolFalseExtractor : public BoolBaseExtractor{
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, bool &result) ;
};
#endif

class DLLEXPORT BoolTrueExtractor : public BoolBaseExtractor{
public:
	static bool getvalue(const char *optname, const char *instring, unsigned int &currentarg, bool &result);
};



class DLLEXPORT OptionBase {
public:
	OptionBase(bool optional_p, const char *flag_p, const char *argname_p, unsigned int propsheet_p, const char *description_p, const char * TeXhelp_p, bool hideInGui_p):
	  flag(flag_p),
	  argname(argname_p),
	  propsheet(propsheet_p),
	  description(description_p),
	  TeXhelp(TeXhelp_p),
	  optional(optional_p),
	  membername(""), // membername is set during "add" because it was simpler to grab the membername during the add
	  hideInGui(hideInGui_p) { 
		assert(flag_p);
		assert(description_p);
		// if (!argname_p) { cerr << "flag " << flag_p << " description " << description_p << endl; }
		assert(argname_p);
	};
	virtual ~OptionBase() { membername = nullptr; }
	virtual ostream & writevalue(ostream & out) const = 0;
	void toString(RSString &) const;  
	std::string valueAsStdString() const;

	virtual bool copyValueFromArgcArgv(const char *optname, const char *valuestring, unsigned int &currentarg) = 0;
	virtual bool copyValueFromString(const char *valuestring) = 0;
	virtual const char *gettypename() const = 0;
	virtual unsigned int gettypeID() const = 0;
	virtual void * GetAddrOfValue() = 0; // will return real type instead of void

	//lint -esym(1540,OptionBase::flag) // not freed
	//lint -esym(1540,OptionBase::description) // not freed
	const char * const flag;		// -bf
	const char * const argname;     // a meaningfull name of the argument (if not a boolean option)
	const unsigned int propsheet;	        // the number of the propertysheet to place this option on
	const char * const description;	// help text
	const char * const TeXhelp; // the long documentation. Will be generated into the overall .tex documentation.
	const bool optional;
	const char * membername; // membername is set during "add" because it was simpler to grab the membername during the add
	const bool hideInGui;
	enum class ctorToUseForValue { useDefaultCtor };
private:
	OptionBase(); // disabled
	OptionBase(const OptionBase&); // disabled
	const OptionBase& operator=(const OptionBase&); // disabled
};

template <class ValueType, class ExtractorType >
class OptionT : public OptionBase {
public:
	OptionT /* < ValueType, ExtractorType > */ (bool optional_p, const char *flag_p, const char *argname_p, unsigned int propsheet_p, const char *description_p, const char * TeXhelp_p, const ValueType & initialvalue, bool hideInGui_p = false)	:
		OptionBase(optional_p, flag_p, argname_p, propsheet_p, description_p, TeXhelp_p, hideInGui_p),
		value(initialvalue) {
	};
	OptionT /* < ValueType, ExtractorType > */ (bool optional_p, const char* flag_p, const char* argname_p, unsigned int propsheet_p, const char* description_p, const char* TeXhelp_p, ctorToUseForValue , bool hideInGui_p = false) :
		OptionBase(optional_p, flag_p, argname_p, propsheet_p, description_p, TeXhelp_p, hideInGui_p),
        value() // use default init for value
	{
			//lint -esym(1401,*::value) // not initialized - we use the default ctor here
	};
	virtual ostream & writevalue(ostream & out) const;

	// used for parsing driver specific options.
	virtual bool copyValueFromArgcArgv(const char *optname, const char *valuestring, unsigned int &currentarg) {
		return ExtractorType::getvalue(optname, valuestring, currentarg, value);
	}

	virtual bool copyValueFromString(const char *valuestring) {
		// just copy the value from a given string, no handling of argc and argv related pointers. 
		unsigned int dummy = 0; // 0 means - use boolean from string
		return ExtractorType::getvalue("no name because of copyvalueFromString", valuestring, dummy, value);
	}
	virtual const char *gettypename() const {
		return ExtractorType::gettypename();
	}
	virtual unsigned int gettypeID() const {
		return ExtractorType::gettypeID();
	}
	//lint -save -esym(1539,OptionBase::optional) // not assigned a value
	//lint -e(1763)
  	ValueType & operator()() { return value; }
	const ValueType & operator()() const { return value; }
	operator ValueType () const { return value; }
	/* const ValueType & */ void operator =(const ValueType & arg) {
		/* return */ value = arg; 
		//lint -esym(1539,OptionBase::propsheet)  // not assigned in op=
		//lint -esym(1539,OptionBase::membername) // not assigned in op=
	} //  cannot return a reference, because char*::operator= does not
	//lint -restore
	bool operator !=(const ValueType & arg) const { return value != arg; }
	bool operator ==(const ValueType & arg) const { return value == arg; }
	bool operator !() const { return !value ; }
	virtual void * GetAddrOfValue() { return &value;}

	ValueType value;

private:
	OptionT() = delete;// disabled
	OptionT(const OptionT&) = delete;// disabled
	const OptionT& operator=(const OptionT&) = delete; // disabled
};


template <class ValueType, class ExtractorType >
ostream & OptionT<ValueType, ExtractorType>::writevalue(ostream & out) const {
		out << value;
		return out;
}

class DLLEXPORT ProgramOptions {
public:
	explicit ProgramOptions(bool expectUnhandled_p = false) : 
		expectUnhandled(expectUnhandled_p), 
		unhandledCounter(0)
	{ 
		unhandledOptions.clear();
		alloptions.clear(); 
	};

	virtual ~ProgramOptions() {
		// cout << "destroyed   options " << this << endl;
	}
	unsigned int parseoptions(ostream & outstr, unsigned int argc, const char * const*argv) ;
	//  int sheet: -1 indicates "all"
	static constexpr int allSheets = -1;
	void showhelp(ostream & outstr, const char* const introText, const char * prefix_for_anchor, bool forTeX, bool withdescription, int sheet ) const ;
	void dumpunhandled(ostream & outstr) const ;	
#if 1
	void showvalues(ostream & outstr, bool withdescription = true) const ;
#endif
	std::vector<OptionBase*>& getOptions(); 
	const std::vector<OptionBase*>& getOptionConst() const; 
	
	size_t numberOfOptions() const;
	const OptionBase* iThOption(unsigned int i) const;
	OptionBase* iThOption(unsigned int i);
	OptionBase* optionByFlag(const char * flag);

	virtual bool hideFromDoku(const OptionBase& /* opt */ ) const { return false; } // some options may be hidden, i.e. debug only options
	virtual bool hideSheetFromGui(unsigned int /* sheet */) const { return false; }

protected:
	void add(OptionBase * op, const char * const membername_p) ;
	unsigned int add_category(const char* category) { 
		categories.push_back(category); 
		return (unsigned int)(categories.size() - 1); // the index
	}
public:
	const char* propSheetName(unsigned int sheet) const;
	void setInputAndOutputFile(const char* const inputFile, const char* const outputFile); // sets to files into unhandledOptions
	const bool expectUnhandled; // whether to expect unhandled arguments
	unsigned int unhandledCounter; //TODO remove this member
	std::vector<const char*> unhandledOptions;
	std::vector<const char*> categories;

  private:
	std::vector<OptionBase*> alloptions;

  private:
	ProgramOptions(const ProgramOptions&) = delete; // disabled
	const ProgramOptions& operator =(const ProgramOptions&) = delete; // disabled
};

#endif
