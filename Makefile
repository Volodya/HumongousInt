CC=g++

CFLAGS=-c -Wall -std=c++17 -lpthread
LDFLAGS=-pthread
SOURCES=$(shell find . -name '*.cpp')
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=run

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo Linking
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	@echo Compiling
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

