HEALPIX_CXX_DIR=/Users/jaz/src/Healpix_3.11/src/cxx

HEALPIX_INCLUDE=-I$(HEALPIX_CXX_DIR)/Healpix_cxx/  -I$(HEALPIX_CXX_DIR)/cxxsupport
HEALPIX_LINK=-L$(HEALPIX_CXX_DIR)/basic_gcc/lib/ -lhealpix_cxx -lcxxsupport -lc_utils -lcfitsio
GL_FLAGS=-framework OpenGL
OCULUS_DIR=/Users/jaz/src/oculus/Oculus\ SDK\ 0.4.4/OculusSDK
OCULUS_INCLUDE=-I $(OCULUS_DIR)/LibOVR/Include/  -I $(OCULUS_DIR)/LibOVR/Src/  
#-F  $(OCULUS_DIR)/LibOVR//Lib/Mac/Release/  -framework LibOVR
OCULUS_LINK=-L$(OCULUS_DIR)/LibOVR/Lib/Mac/Release -lovr -framework Foundation -framework CoreGraphics -framework IOKit
#-F  $(OCULUS_DIR)/LibOVR//Lib/Mac/Release/  -framework LibOVR

PNG_DIR=/opt/X11
PNG_INCLUDE=-I$(PNG_DIR)/include
PNG_LINK=-L$(PNG_DIR)/lib -l png
CXXFLAGS=-g -O0 -Wno-deprecated-declarations -std=c++11 -Wno-write-strings $(HEALPIX_INCLUDE) $(OCULUS_INCLUDE) $(PNG_INCLUDE)
LDFLAGS=$(CXXFLAGS) $(GL_FLAGS) $(HEALPIX_LINK) -lglew -lglfw3 $(PNG_LINK)


CXX=g++

OBJ=avr_object.o avr_sphere.o avr_healpix.o avr_catalog.o avr_viewer.o avr_image.o avr_test.o avr_fits.o  color_map.o avr_gl.o

oculus: avr_oculus.cc FBO.cc $(OBJ)
	$(CXX) avr_oculus.cc FBO.cc  $(OBJ) -o oculus $(HEALPIX_INCLUDE) $(CXXFLAGS) $(OCULUS_INCLUDE)  $(LDFLAGS) $(OCULUS_LINK) $(HEALPIX_LINK)



%.o: %.cc
	$(CXX) $(CXXFLAGS) $(HEALPIX_INCLUDE) -c  $<

main: $(OBJ) main.cc
	$(CXX) $(CXXFLAGS) -o main main.cc $(OBJ)  $(LDFLAGS)


temp: temp.cc
	$(CXX) temp.cc -o temp $(CXXFLAGS) $(OCULUS_INCLUDE)  $(LDFLAGS) $(OCULUS_LINK)

test: test.cc avr_gl.o
	$(CXX) test.cc -o test avr_gl.o $(CXXFLAGS) $(OCULUS_INCLUDE) $(LDFLAGS) $(OCULUS_LINK)
clean:
	rm -rf $(OBJ) main oculus *.dSYM

#g++ -o main main.o color_map.o  avr_image.o avr_viewer.o avr_healpix.o avr_fits.o avr_catalog.o avr_object.o ${GL_FLAGS} ${LDFLAGS}