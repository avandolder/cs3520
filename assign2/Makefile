CXX=clang++-9
CXXFLAGS=-Wall -O2
RM=rm -f
LDLIBS=-lglut -lGL -lGLU

SRCS=main.cpp state.cpp
OBJS=$(subst .cpp,.o,$(SRCS))
NAME=assign2

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) -o $(NAME) $(OBJS) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) $(NAME)
