all: race rp

race: race.o
	gcc -o race race.o

race.o: race.c
	gcc -c race.c

rp: raceproc.o
	gcc -o rp raceproc.o

raceproc.o: raceproc.c
	gcc -c raceproc.c

clean:
	-rm *.o race rp
