CXXFLAGS =\
 -g\
 -pipe\
 -std=c++1y\
 -O2\
 -fstrict-aliasing\
 -fno-common\
 -pedantic\
 -Wall\
 -Wextra

INCLUDES =\
 `sdl-config --cflags`\
 `Magick++-config --cppflags`

LDFLAGS =\
 -Wl,--enable-new-dtags\
 -Wl,--sort-common

LIBS =\
 -lzip\
 -lGL\
 -lGLU\
 `sdl-config --libs`\
 `Magick++-config --libs`

CXXSRCS =\
 main.cc\
 bezier.cc\
 bsp.cc\
 frustum.cc\
 tex.cc\
 vec.cc\
 zipfile.cc

OBJS = $(CXXSRCS:.cc=.o)
EXE = q3bsp

all: $(EXE)

depend:
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MM $(CXXSRCS)

$(EXE): $(OBJS)
	$(CXX) $(LDFLAGS) -o $(EXE) $(OBJS) $(LIBS)

.SUFFIXES: .cc

.cc.o:
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(EXE) $(OBJS)

# DEPENDENCIES
# ===========================================================================
