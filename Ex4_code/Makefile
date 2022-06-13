# Makefile for TCP project

all: measure sender

tcp-server: measure.c
	gcc -o measure measure.c

tcp-client: sender.c
	gcc -o sender sender.c

clean:
	rm -f *.o measure sender

runs:
	./measure

runc:
	./sender

runs-strace:
	strace -f ./measure

runc-strace:
	strace -f ./sender