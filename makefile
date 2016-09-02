all:
	gcc -o nInLine nInLineBack.c nInLineFront.c
	
	
clean:
	rm -f nInLine

