CC=g++
CFLAGS=-std=c++11 -ggdb -Wall -pthread
OFLAGS=-c $(CFLAGS)
OBJ=main.o bot.o config.o privleges.o miscbotlib.o cmutex.o net.o \
english.o quote.o userdb.o

all: ibot

ibot: $(OBJ)
	$(CC) $(OBJ) $(CFLAGS) -o ibot

main.o: main.cpp
	$(CC) $(OFLAGS) main.cpp

%.o: %.cpp %.h
	$(CC) $(OFLAGS) -o $@ $<

bin:
	mkdir -p bin

.PHONY: clean cleanobj

clean: cleanobj
	rm -rf ibot

cleanobj:
	rm -rf *.o
