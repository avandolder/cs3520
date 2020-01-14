CXX=g++
CXXFLAGS=-std=c++11
RM=rm -f
LDLIBS=-lglut -lGL -lGLU

SRCS=draw_line.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: draw_line

draw_line: $(OBJS)
	$(CXX) -o draw_line $(OBJS) $(LDLIBS)

%.o: %.cpp
	g++ $(CXXFLAGS) -c $<

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) draw_line
