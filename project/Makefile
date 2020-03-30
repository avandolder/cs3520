CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -pedantic -O2
RM=rm -f
LDLIBS=-lglut -lGL -lGLU

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
NAME=project

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) -o $(NAME) $(OBJS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(NAME)
