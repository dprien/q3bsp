# $Id$

CXXFLAGS =\
 -std=c++14\
 -pipe\
 -g\
 -O2\
 -ftracer\
 -fomit-frame-pointer\
 -fvisibility-inlines-hidden\
 -fno-common\
 -fstrict-aliasing\
 -std=c++98\
 -pedantic\
 -Wall\
 -Wextra

CFLAGS =\
 -std=c90\
 -pipe\
 -g\
 -O2\
 -ftracer\
 -fomit-frame-pointer\
 -fno-common\
 -fstrict-aliasing

INCLUDES = `sdl-config --cflags` `Magick++-config --cppflags`

LDFLAGS = `sdl-config --libs` `Magick++-config --ldflags` -Wl,-O1 -Wl,--relax -Wl,--enable-new-dtags -Wl,--sort-common
LIBS = `Magick++-config --libs` -lz -lGL -lGLU

EXE = q3bsp

OBJ =\
 vec.o\
 bsp.o\
 zipfile.o\
 tex.o\
 bezier.o\
 frustum.o\
 main.o\
 ioapi.o\
 unzip.o

all : $(EXE)

$(EXE) : $(OBJ)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJ) $(LIBS)

main.o : main.cc exception.h sdlsentry.h bsp.h vec.h zipfile.h tex.h frustum.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c main.cc

bsp.o : bsp.cc bsp.h exception.h ibsp46.h tex.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c bsp.cc

bsp.h : ibsp46.h vec.h zipfile.h bezier.h frustum.h
	touch bsp.h

vec.o : vec.cc vec.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c vec.cc

ibsp46.h : types.h ibsp.h
	touch ibsp46.h

ibsp.h : types.h
	touch ibsp.h

sdlsentry.h : exception.h
	touch sdlsentry.h

zipfile.o : zipfile.cc zipfile.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c zipfile.cc

zipfile.h : types.h zlib/contrib/minizip/unzip.h
	touch zipfile.h

tex.o : tex.cc tex.h exception.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) `Magick++-config --cppflags --cxxflags` -c tex.cc

tex.h : types.h zipfile.h ibsp46.h
	touch tex.h

bezier.o : bezier.cc bezier.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c bezier.cc

bezier.h : ibsp46.h
	touch bezier.h

frustum.o : frustum.cc frustum.h
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c frustum.cc

frustum.h : vec.h
	touch frustum.h

# Zlib's minizip contribution
# =========================================================================
unzip.o : zlib/contrib/minizip/unzip.c zlib/contrib/minizip/unzip.h zlib/contrib/minizip/crypt.h
	$(CC) $(CFLAGS) $(INCLUDES) -c zlib/contrib/minizip/unzip.c

zlib/contrib/minizip/unzip.h : zlib/contrib/minizip/ioapi.h
	touch zlib/contrib/minizip/unzip.h

ioapi.o : zlib/contrib/minizip/ioapi.c zlib/contrib/minizip/ioapi.h
	$(CC) $(CFLAGS) $(INCLUDES) -c zlib/contrib/minizip/ioapi.c
# =========================================================================

clean :
	rm -f $(EXE)
	rm -f $(OBJ)
