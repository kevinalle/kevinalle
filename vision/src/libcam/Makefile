CXXFLAGS=-fPIC -O4 `pkg-config opencv --cflags` -Wall #-msoft-float -march=armv4t
LDFLAGS=`pkg-config opencv --libs` -fPIC -shared 

all: libcam.so

libcam.so: libcam.o
	g++ -o libcam.so $< $(LDFLAGS)

clean:
	rm -f *.o *.so





