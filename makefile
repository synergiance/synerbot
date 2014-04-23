CC=g++
CFLAGS=-c -std=c++11

all: ibot

ibot: main.o bot.o config.o privleges.o miscbotlib.o
	$(CC) main.o bot.o config.o privleges.o miscbotlib.o -o ibot

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

bot.o: bot.cpp bot.h
	$(CC) $(CFLAGS) bot.cpp

config.o: config.cpp config.h
	$(CC) $(CFLAGS) config.cpp

privleges.o: privleges.cpp privleges.h
	$(CC) $(CFLAGS) privleges.cpp

miscbotlib.o: miscbotlib.cpp miscbotlib.h
	$(CC) $(CFLAGS) miscbotlib.cpp

bin:
	mkdir -p bin

.PHONY: clean cleanobj

clean: cleanobj
	rm -rf ibot

cleanobj:
	rm -rf *.o