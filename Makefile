HEALPIX_CXX_DIR=/Users/jaz/src/Healpix_3.11/src/cxx

HEALPIX_INCLUDE=-I$(HEALPIX_CXX_DIR)/Healpix_cxx/  -I$(HEALPIX_CXX_DIR)/cxxsupport
HEALPIX_LINK=-L$(HEALPIX_CXX_DIR)/basic_gcc/lib/ -lhealpix_cxx -lcxxsupport -lc_utils -lcfitsio
GL_FLAGS=-framework OpenGL
CXXFLAGS=-g -O0 -Wno-deprecated-declarations -std=c++11 -Wno-write-strings
LDFLAGS=$(CXXFLAGS) $(GL_FLAGS) $(HEALPIX_LINK) -lglew -lglfw3
CXX=g++

OBJ=avr_object.o avr_healpix.o avr_catalog.o avr_viewer.o avr_image.o avr_fits.o main.o color_map.o  

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(GL_FLAGS) $(HEALPIX_INCLUDE) -c  $<

main: $(OBJ)
	$(CXX) $(CXXFLAGS) -o main $(OBJ) $(GL_FLAGS) $(LDFLAGS)

clean:
	rm -f $(OBJ) main

#g++ -o main main.o color_map.o  avr_image.o avr_viewer.o avr_healpix.o avr_fits.o avr_catalog.o avr_object.o ${GL_FLAGS} ${LDFLAGS}