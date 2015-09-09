HEALPIX_CXX_DIR=/Users/jaz/src/Healpix_3.11/src/cxx

HEALPIX_INCLUDE=-I$(HEALPIX_CXX_DIR)/Healpix_cxx/  -I$(HEALPIX_CXX_DIR)/cxxsupport
HEALPIX_LINK=-L$(HEALPIX_CXX_DIR)/basic_gcc/lib/ -lhealpix_cxx -lcxxsupport -lc_utils -lcfitsio
GL_FLAGS=-framework OpenGL
OCULUS_DIR=/Users/jaz/src/oculus/OculusSDK\ 2
OCULUS_INCLUDE=-I $(OCULUS_DIR)/LibOVR/Include/  -I $(OCULUS_DIR)/LibOVRKernel/Src/  -F  $(OCULUS_DIR)/LibOVR//Lib/Mac/Release/  -framework LibOVR
OCULUS_LINK=-F  $(OCULUS_DIR)/LibOVR//Lib/Mac/Release/  -framework LibOVR

CXXFLAGS=-g -O0 -Wno-deprecated-declarations -std=c++11 -Wno-write-strings $(HEALPIX_INCLUDE) $(OCULUS_INCLUDE) $(GL_FLAGS)
LDFLAGS=$(CXXFLAGS) $(GL_FLAGS) $(HEALPIX_LINK) -lglew -lglfw3


CXX=g++

OBJ=avr_object.o avr_healpix.o avr_catalog.o avr_viewer.o avr_image.o avr_test.o avr_fits.o  color_map.o avr_gl.o



%.o: %.cc
	$(CXX) $(CXXFLAGS) $(GL_FLAGS) $(HEALPIX_INCLUDE) -c  $<

main: $(OBJ)
	$(CXX) $(CXXFLAGS) -o main main.cc $(OBJ) $(GL_FLAGS) $(LDFLAGS)

oculus: avr_oculus.cc FBO.cc $(OBJ)
	$(CXX) avr_oculus.cc FBO.cc  $(OBJ) -o oculus $(HEALPIX_INCLUDE) $(CXXFLAGS) $(OCULUS_INCLUDE) $(GL_FLAGS) $(LDFLAGS) $(OCULUS_LINK) $(HEALPIX_LINK)

temp: temp.cc
	$(CXX) temp.cc -o temp $(CXXFLAGS) $(OCULUS_INCLUDE) $(GL_FLAGS) $(LDFLAGS) $(OCULUS_LINK)

test: test.cc avr_gl.o
	$(CXX) test.cc -o test avr_gl.o $(CXXFLAGS) $(OCULUS_INCLUDE) $(GL_FLAGS) $(LDFLAGS) $(OCULUS_LINK)
clean:
	rm -f $(OBJ) main

#g++ -o main main.o color_map.o  avr_image.o avr_viewer.o avr_healpix.o avr_fits.o avr_catalog.o avr_object.o ${GL_FLAGS} ${LDFLAGS}