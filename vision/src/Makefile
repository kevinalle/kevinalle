CXXFLAGS := -g -O3 -fPIC -Wall `pkg-config --cflags opencv` -Ilibelas/src -msse2
#LDFLAGS := `pkg-config --libs opencv` -L./libelas/build/ -lelas
LDFLAGS := -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_legacy -lopencv_flann -L./libelas/build/ -lelas #-L./libexabot-remote/ -lexabot-remote

all: libelas/build/libelas.a main

libelas/build/libelas.a:
	cd libelas; mkdir build; cmake ..; make

clean:
	rm -rf *.o main

main: main.o
	g++ -o $@ main.o $(LDFLAGS)

