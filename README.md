
# pstoedit
pstoedit - translates PostScript graphics into other vector formats.
Emphasis is on **vector** formats. There are many other tools based on pure raster image formats but conversion to vector formats retaining high level vector information is more complex and not many tools are available for this type of conversion.


## Further Documentation

[Documentation](doc/pstoedit.htm)


## Dependendies
### Build time Dependendies

The configure script checks for the availability of several third party components and configures the build process accordingly. The following components are being checked:

* pdflatex for the generation of the manual - Mandatory
* libplot and libplotter for using the additional backends provided by those libs (apt name libplot-dev) - Optional. When avaailable adds a bunch of more backend options via the libplotter.
* QT for GUI - Optional
* libzip for ppt backend (apt name libzip-dev) - Optional. When available the PPTX (PowerPoint) backend is enabled.
* libemf for enhanced windows metafiles (apt name libemf-dev) - Optional. Needed for the EMF backend.
* libmagic for Magic drivers - Optional. Provides access to drivers provided by the libmagic.

see also the [recommended apt packages](doc/pstoedit.linux.prerequisites.txt)


### Run time Dependendies

The most important one is GhostScript. You need a working installation of this PostScript interpreter.

## Installation
### Installation in Linux

To build pstoedit from source code run
```bash
  sh autogen.sh
  ./configure
  make; make install
```
from the root directory of the project.
The first command will first update all the autotools related files needed by the subsequent configure and make.
pstoedit still uses the classical autotools instead of Cmake and the like.

## Cleanup
After a full build you can use the following steps to remove all the files which were generated during the build.
```bash
  make clean
  sh rm_generated.sh
```

### Installation in Windows

winget install pstoedit

So far a binary is provided only for Windows, i.e no make system despite the availability of the source code. This is because the provided binary is based on several third party libraries and building them upfront is sometimes quite complex.
## License

[GNU GPLv2](https://choosealicense.com/licenses/gpl-2.0/)

## Web site

See here for more information - [pstoedit.com](https://pstoedit.com/).

## Support

For support, email wglunz35_AT_pstoedit.net.

If you want to support the development of pstoedit any further or just find the program very useful consider a small donation.
