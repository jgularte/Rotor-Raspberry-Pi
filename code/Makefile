
CC = gcc
ODIR = obj
SDIR = src
DOCDIR = doc
INC = -Iinclude
LIBS = -lpthread -lwiringPi -lm
CFLAGS = -Wall
BINS = diagpov led


_OBJ_LED = motor.o utils.o web_client.o led.o


OBJS_DIAP = $(patsubst %,$(ODIR)/%,$(_OBJ_DIAP))
OBJS_LED = $(patsubst %,$(ODIR)/%,$(_OBJ_LED))

$(ODIR)/%.o: $(SDIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) $(LIBS)

diagpov: $(OBJS_DIAP)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

led: $(OBJS_LED)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: $(BINS)

.PHONY: doc
doc:
	doxygen

clean:
	rm -Rf $(ODIR) $(BINS) $(DOCDIR)
