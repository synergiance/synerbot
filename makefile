CC=g++
CFLAGS=-c -std=c++11
OBJ=main.o bot.o config.o privleges.o miscbotlib.o cmutex.o net.o

all: ibot

ibot: $(OBJ)
	$(CC) $(OBJ) -o ibot

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

%.o: %.cpp %.h
	$(CC) $(CFLAGS) -o $@ $<

bin:
	mkdir -p bin

.PHONY: clean cleanobj

clean: cleanobj
	rm -rf ibot

cleanobj:
	rm -rf *.o
