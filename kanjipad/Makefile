### C options ###
# These flags will be used for compiling the kpengine program, upon which our
# character handwriting recognition is dependent.
CC = gcc
CFLAGS = -Wall -s -O2

$(kpengine) : kpengine.o scoring.o util.o
	$(mkdircmd) $(BINDIR)
	$(CC) $(CFLAGS) -o $(kpengine) kpengine.o scoring.o util.o
kpengine.o: kanjipad/kpengine.c
	$(CC) $(CFLAGS) -c -o kpengine.o -DFOR_PILOT_COMPAT -Ikanjipad kanjipad/kpengine.c
scoring.o: kanjipad/scoring.c
	$(CC) $(CFLAGS) -c -o scoring.o -DFOR_PILOT_COMPAT -Ikanjipad/jstroke kanjipad/scoring.c
util.o: kanjipad/util.c
	$(CC) $(CFLAGS) -c -o util.o -DFOR_PILOT_COMPAT -Ikanjipad/jstroke kanjipad/util.c
