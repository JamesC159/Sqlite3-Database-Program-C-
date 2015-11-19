CC=g++
INSTDIR=.
CFLAGS=-c -Wall -g
LDFLAGS=-lsqlite3
SOURCES=main.cpp Artist.cpp Track.cpp Cd.cpp
HEADERS=Artist.hpp Track.hpp Cd.hpp
OBJECTS=$(SOURCES:.cpp=.o)
FILES=song.db
EXECUTABLE=driver

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

main.o: main.cpp $(HEADERS)
	$(CC) $(LDFLAGS) $(CFLAGS) main.cpp

Artist.o: Artist.cpp Artist.hpp
	$(CC) $(CFLAGS) Artist.cpp

Track.o: Track.cpp Track.hpp
	$(CC) $(CFLAGS) Track.cpp
 
Cd.o: Cd.cpp Cd.hpp
	$(CC) $(CFLAGS) Cd.cpp

clean:
	-rm *.o $(FILES) $(EXECUTABLE)
