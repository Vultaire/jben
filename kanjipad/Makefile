### C options ###
# These flags will be used for compiling the kpengine program, upon which our
# character handwriting recognition is dependent.
CC = gcc
CFLAGS = -Wall -s -O2

kpadsources = $(shell ls -t kanjipad/*.c) # Compile most recently edicted files first.
kpadobjects = $(sources:%.c=$(OBJDIR)/%.o)

$(kpengine) : $(kpadobjects)
	$(mkdircmd) $(BINDIR)/kanjipad
	$(CC) $(CFLAGS) -o $(kpengine) $(OBJDIR)/kanjipad/kpengine.o $(OBJDIR)/kanjipad/scoring.o $(OBJDIR)/kanjipad/util.o

$(OBJDIR)/kanjipad/%.o: kanjipad/%.c
	$(mkdircmd) $(OBJDIR)/kanjipad
	$(CC) $(CFLAGS) -c -o $@ -DFOR_PILOT_COMPAT -Ikanjipad $<
