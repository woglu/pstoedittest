#ifndef BASE64WRITER
#define BASE64WRITER
/* 
	Base64 encoding - originally part of drvSK (by Bernhard Herzog) - now factored out for general usage

    This base64 code is a modified version of the code in Python's
    binascii module, which came with the following license:

    Copyright 1991, 1992, 1993, 1994 by Stichting Mathematisch Centrum,
Amsterdam, The Netherlands.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Stichting Mathematisch
Centrum or CWI or Corporation for National Research Initiatives or
CNRI not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior
permission.

While CWI is the initial source for this software, a modified version
is made available by the Corporation for National Research Initiatives
(CNRI) at the Internet address ftp://ftp.python.org.

STICHTING MATHEMATISCH CENTRUM AND CNRI DISCLAIM ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH
CENTRUM OR CNRI BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.


*/


#ifndef cppcomp_h
#include "cppcomp.h"
#endif

#include I_iostream
//#include I_fstream

USESTD


class DLLEXPORT Base64Writer {
public:
	explicit Base64Writer( ostream& outf_p) : outf(outf_p), leftbits(0), leftchar(0), column(0), closed(0) {}
	~Base64Writer();
private:
	ostream & outf;
	unsigned int leftbits;
	unsigned int leftchar;
	unsigned int column;
	int closed;
   
public:
    int write_base64(const unsigned char * buf, size_t length);
    void close_base64();

private:
	// declared but not defined
	NOCOPYANDASSIGN(Base64Writer)
	//Base64Writer(const Base64Writer&);
	Base64Writer() = delete;
	//const Base64Writer& operator=(const Base64Writer&);
};

#endif
