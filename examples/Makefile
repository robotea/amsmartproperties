CXX = g++

CXXFLAGS = -O0 -ggdb -I../include -L../build/bin/
RM = rm -f
OUTPUTDIR = ../build/bin/examples
GARBAGE = *.o *.~*

#Program modules

LIBS = AMSmartProperties

#`pkg-config --libs opencv` -lswscale

# $@ - the current target
# $? - dependencies change from the last time?
# $^ - all dependencies without duplicates
# $+ - all dependencies
# %  - wildcard :
# %.c:
# 		gcc -o $* $*.c

.PHONY: all clean

all: remove createdir ../build/bin/examples/AMFunction_1 ../build/bin/examples/AMProperty_1 ../build/bin/examples/AMProperty_2 ../build/bin/examples/AMProperty_3
#all: remove ./bin/stringtable_c

#test:  test.o Etypes.o
#	$(CXX) $+ -o $@  $(CXXFLAGS) $(LIBS)

#.o: .cc
#	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LIBS)

remove:
	rm -f ../build/bin/examples/*

createdir:
	mkdir -p ../build/bin/examples

../build/bin/examples/AMFunction_1: AMFunction_1.cpp
	$(CXX) $(CXXFLAGS) $+ -o $@ -l$(LIBS)

../build/bin/examples/AMProperty_1: AMProperty_1.cpp
	$(CXX) $(CXXFLAGS) $+ -o $@ -l$(LIBS)

../build/bin/examples/AMProperty_2: AMProperty_2.cpp
	$(CXX) $(CXXFLAGS) $+ -o $@ -l$(LIBS)

../build/bin/examples/AMProperty_3: AMProperty_3.cpp
	$(CXX) $(CXXFLAGS) $+ -o $@ -l$(LIBS)
clean:
	$(RM) ../build/bin/examples/*
