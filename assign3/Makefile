CXX=g++
CXXFLAGS=-std=c++11 -Wall -O2
RM=rm -f
LDLIBS=-lglut -lGL -lGLU

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
NAME=assign3

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) -o $(NAME) $(OBJS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(NAME)
