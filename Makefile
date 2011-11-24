CC	= gcc

CFLAGS	= -Wall -g
LDFLAGS	= #-lpthread -lm

TARGETS = #add target here

.PHONY: all clean

all:$(TARGETS)

clean:
	rm -rf	*~ $(TARGETS) .*.*.swp .*.*.swo *.o
