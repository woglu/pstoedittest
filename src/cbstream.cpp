/* 

   callbackBuffer : This file is part of pstoedit
   streambuf that writes the data to a user defineable call back function

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


This code is mainly based on the example given in:

The GNU C++ Iostream Library
	Using the streambuf Layer 
		Simple output re-direction by redefining overflow 


*/

#include "cppcomp.h"

#include "cbstream.h"
#include I_string_h

int callbackBuffer::underflow(void)
{
	const char message[] = "unexpected call to callbackbuf::underflow()\n";
	(void) write_to_callback(message, strlen(message));
	return 0;
}

write_callback_type *callbackBuffer::set_callback(void *cb_data_p, write_callback_type * new_cb)
{
	write_callback_type *old_cb = write_callback;
	write_callback = new_cb;
	cb_data = cb_data_p;
	return old_cb;
}

int callbackBuffer::write_to_callback(const char *text, size_t length)
{
	if (write_callback) {
		return (*write_callback) (cb_data, text, (int) length); // cast to int - see comment in cbstream.h
	} else {
		return 0;
	}
}

int callbackBuffer::sync()
{
	const std::streamsize n = pptr() - pbase();
	return (n && write_to_callback(pbase(), (unsigned int) n) != n) ? EOF : 0;
}

int callbackBuffer::overflow(int ch)
{
	const std::streamsize n = pptr() - pbase();
	if (n && sync())
		return EOF;
	if (ch != EOF) {
		char cbuf[1];
		cbuf[0] = (char) ch;
		if (write_to_callback(cbuf, 1) != 1)
			return EOF;
	}
	pbump(-n);					// Reset pptr().
	return 0;
}

std::streamsize callbackBuffer::xsputn(const char *text, std::streamsize n)
{
	return sync() == EOF ? 0 : write_to_callback(text, (unsigned int) n);
}


// #define TEST
#ifdef TEST

int write_callback_test(void *cb_data, const char *text, int length)
/* Returns number of characters successfully written  */
{
//  cout << "@@@@" << length << endl;
	unsigned int count = length;
	while (count) {
		cout << *text;
		text++;
		count--;
	}
	return length;
}


int main(int argc, char **argv)
{

	callbackBuffer wbuf(0, write_callback_test);

//  ostream wstr(&wbuf);
	cerr = &wbuf;
	cerr << "Hello world!\n" << endl;
	wbuf.set_callback(0, 0);
	cerr << "Hello world!\n";
	return 0;
}

#endif
