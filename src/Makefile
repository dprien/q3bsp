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
 archive.cc\
 time.cc

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
main.o: main.cc exception.h bsp.h ibsp46.h ibsp.h vec.h archive.h \
 bezier.h frustum.h tex.h image.h time.h
image.o: image.cc image.h exception.h
time.o: time.cc time.h
bezier.o: bezier.cc bezier.h ibsp46.h ibsp.h
bsp.o: bsp.cc bsp.h ibsp46.h ibsp.h vec.h archive.h bezier.h frustum.h \
 exception.h tex.h image.h
frustum.o: frustum.cc frustum.h vec.h
tex.o: tex.cc tex.h ibsp46.h ibsp.h archive.h image.h exception.h
vec.o: vec.cc vec.h
archive.o: archive.cc exception.h archive.h
