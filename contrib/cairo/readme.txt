This directory contains an example EPS file along with a sample C program and makefile.
These are related to the cairo backend of pstoedit. 
After building pstoedit, you can do

make -f cairo-example.mk

and it should call pstoedit to produce cairo code and then it compiles 
it along with a simple top level program that will render the image both 
to X11 as well as to a PDF file.


Dan McMahill
