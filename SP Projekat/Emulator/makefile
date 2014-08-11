VPATH = /usr/include

Emulator: Core.o Instructions.o main.o Utility.o
	gcc -o Emulator -g Core.o Instructions.o main.o Utility.o

Core.o: Core.c Core.h stdio.h Utility.h Instructions.h
	gcc -c -g Core.c

Instructions.o: Instructions.c Instructions.h Utility.h Core.h
	gcc -c -g Instructions.c -masm=intel

Utility.o: Utility.c Utility.h
	gcc -c -g Utility.c

main.o: main.c stdio.h stdlib.h Core.h
	gcc -c -g main.c

clean:
	rm Core.o Instructions.o Utility.o main.o
