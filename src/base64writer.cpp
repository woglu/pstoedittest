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

#include "base64writer.h"


static const unsigned short BASE64_MAXASCII=76;		/* Max chunk size (76 char line) */

static const unsigned char table_b2a_base64[] =
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

#define FBUFLEN 2048
#define BASE64_PAD '='

int Base64Writer::write_base64(const unsigned char *buf, size_t length)
{
	unsigned char encoded[FBUFLEN];
	unsigned char *ascii_data = encoded;
	const unsigned char *bin_data = buf;
	unsigned char this_ch;

	size_t bin_len = ((FBUFLEN / 4) * 3);

	if (bin_len > length)
		bin_len = length;

	/* first, fill the ascii buffer and don't care about max. line length */
	for (; bin_len > 0; bin_len--, bin_data++) {
		/* Shift the data into our buffer */
		leftchar = (leftchar << 8) | *bin_data;
		leftbits += 8;

		/* See if there are 6-bit groups ready */
		while (leftbits >= 6) {
			this_ch = (leftchar >> (leftbits - 6)) & 0x3f;
			leftbits -= 6;
			*ascii_data++ = table_b2a_base64[this_ch];
		}
	}

	/* now output the ascii data line by line */
	size_t ascii_left = ascii_data - encoded;
	while (ascii_left > 0) {
		size_t linelength = BASE64_MAXASCII - column;

		if (ascii_left < linelength)
			linelength = ascii_left;
		(void)outf.write((char *) (ascii_data - ascii_left), linelength);	// SUN CC needs this cast
		ascii_left -= linelength;
		column += linelength;
		if (column >= BASE64_MAXASCII) {
			(void)outf.put('\n');
			column = 0;
		}
	}
	return bin_data - (const unsigned char *) buf;
}

void Base64Writer::close_base64()
{
	if (!closed) {
		unsigned char buf[4];
		unsigned char *ascii_data = buf;

		if (leftbits == 2) {
			*ascii_data++ = table_b2a_base64[(leftchar & 3) << 4];
			*ascii_data++ = BASE64_PAD;
			*ascii_data++ = BASE64_PAD;
		} else if (leftbits == 4) {
			*ascii_data++ = table_b2a_base64[(leftchar & 0xf) << 2];
			*ascii_data++ = BASE64_PAD;
		}

		if (ascii_data > buf || column != 0) {
			*ascii_data++ = '\n';	/* Append a courtesy newline */
		}
		if (ascii_data > buf) {
			(void)outf.write((char *) buf, ascii_data - buf);
		}
		closed = 1;
	}
}

Base64Writer::~Base64Writer()
{
	if (!closed)
		close_base64();
}
