src = $(wildcard *.cpp)
obj = $(src:.cpp=.o)
DEPS = $(wildcard *.h)

CC = g++
CFLAGS = -std=c++11 -I/usr/local/include/opencv -I/usr/local/include/opencv2 -L/usr/local/lib -L/opt/vc/lib
LIBS = -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_imgcodecs -lraspicam -lraspicam_cv -lmmal -lmmal_core -lmmal_util
# -lopencv_tracking 

# Object file rule to ensure compilation is done if header files change
# $< is the first item in the dependency list
%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

# Primary compilation rule
tracker: $(obj)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Clean rule
.PHONY: clean
clean:
	rm -f $(obj) tracker