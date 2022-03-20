PREFIX=m68k-atari-mint
CC=$(PREFIX)-gcc-4.6.4

CFLAGS=-std=gnu99 -Wall -Wno-unused-value -Os -mshort -mfastcall #-mpcrel

LIBCMINI_DIR=$(HOME)/src/libcmini/build
LINK=-nostdlib $(LIBCMINI_DIR)/crt0.o $< -L$(LIBCMINI_DIR)/mshort/mfastcall $(LDFLAGS) -lcmini -lgcc -o $@
# Use this to link with stock mintlib, but it crashes for me currently
#LINK=$< $(LDFLAGS) -o $@

TARGETS=sample.prg

src = $(wildcard *.c)
obj = $(src:.c=.o)

.PHONY: all clean
all: $(TARGETS)
clean:
	rm -f $(TARGETS) $(obj)

sample.prg: sample.c
	$(CC) $(CFLAGS) $(LINK)
	$(PREFIX)-objdump -drwC $@ > $@.s
	$(PREFIX)-strip -s $@
	$(PREFIX)-size $@
