.PHONY = all clean

CC = mpicc

CFLAGS = -O2 -lm -Wall
TFLAGS = -lm

SOURCE := $(wildcard *.c)
BIN := $(SOURCE:%.c=%)

all: ${BIN}

clean: 
	rm -v ${BIN}