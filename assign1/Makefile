CXX=g++
CXXFLAGS=-std=c++11 -O2 `pkg-config --cflags glfw3`
RM=rm -f
LDLIBS=-lGL -lGLU `pkg-config --libs glfw3`

SRCS=assign1.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: assign1

assign1: $(OBJS)
	$(CXX) -o assign1 $(OBJS) $(LDLIBS)

%.o: %.cpp
	g++ $(CXXFLAGS) -c $<

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) assign1
