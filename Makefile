CC=gcc
CXX=g++
CFLAGS=-c -Wall
#Link with Gtkmm 3.0 and libcurl
LDFLAGS= `pkg-config --cflags --libs gtkmm-3.0` -lcurl
SOURCES=src/main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE_FOLDER=build/$(shell uname)
EXECUTABLE=build/$(shell uname)/$(shell basename $(CURDIR))
INCLUDE_FLAGS=-Iinclude

all: $(EXECUTABLE_FOLDER) remove-executable $(EXECUTABLE)

$(EXECUTABLE):
	$(CXX) $(INCLUDE_FLAGS) $(SOURCES) -o $@ $(LINK_FLAGS) $(LDFLAGS)

$(EXECUTABLE_FOLDER):
	mkdir -p $(EXECUTABLE_FOLDER)

clean:
	rm -rf build/*

windows:
	make -f Makefile.windows

remove-executable:
	rm -rf $(EXECUTABLE)