CXX = g++
INCLUDES= -I./
CXXFLAGS = -g $(INCLUDES)
SRCM= ../cbuffer.cpp
OBJM = $(SRCM:.cpp=.o)
LINKFLAGS= -lcppunit

test: test.cpp $(OBJM)
	$(CXX) $(CXXFLAGS) -o $@ test.cpp $(OBJM) $(LINKFLAGS) $(LINKFLAGSLOG4) $(LIBLOG)

# Default compile

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@
