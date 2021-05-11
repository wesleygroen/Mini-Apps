# .PHONY = all clean

# CC = mpicc

# CFLAGS = -O2 -lm
# TFLAGS = -lm

# SOURCE := $(wildcard *.c)
# BIN := $(SOURCE:%.c=%)

# all: ${BIN}

# clean: 
# 	rm -v ${BIN}

CC=mpicc
CFLAGS = -O2 -lm
TFLAGS = -lm
CCFLAGS= -Wall
LDFLAGS=
SOURCES=$(wildcard *.c)
OBJECTS=$(SOURCES:.c=.o)
TARGET=

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) -o $@ $^ $(LDFLAGS) 

%.o: %.c %.h
		$(CC) $(CCFLAGS) -c $<

%.o: %.c
		$(CC) $(CCFLAGS) -c $<

clean:
		rm -f *.o $(TARGET)