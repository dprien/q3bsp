CXXFLAGS =\
 -std=c++14\
 -pipe\
 -fno-strict-aliasing\
 -fno-common\
 -O2\
 -g\
 -pedantic\
 -Wall\
 -Wextra

INCLUDES =\
 `sdl2-config --cflags`

LDFLAGS =\
 -Wl,--enable-new-dtags\
 -Wl,--sort-common

LIBS =\
 -lboost_system\
 -lboost_filesystem\
 -lzip\
 -lGL\
 -lGLU\
 `sdl2-config --libs`\
 -lSDL2_image

CXXSRCS =\
 main.cc\
 image.cc\
 bezier.cc\
 bsp.cc\
 frustum.cc\
 tex.cc\
 vec.cc\
 archive.cc

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
