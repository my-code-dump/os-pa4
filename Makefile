all: race

race: race.o
	gcc -o race race.o

race.o: race.c
	gcc -c race.c

clean:
	-rm *.o race
