CC=g++
CFLAGS=-c

all: ibot

ibot: main.o bot.o
	$(CC) main.o bot.o -o ibot

main.o: main.cpp bot.h
	$(CC) $(CFLAGS) main.cpp

bot.o: bot.cpp bot.h
	$(CC) $(CFLAGS) bot.cpp

bin:
	mkdir -p bin

clean:
	rm -rf *o ibot
