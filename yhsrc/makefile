# Yongzheng Huang, 445575 Operating Systems Project 1

sish: sish.c
	gcc -Wall -g sish.c -o sish

exp: exp.c
	gcc -Wall -g exp.c -o exp

cmlparser: cmlparser.c shell.h
	gcc -Wall -g cmlparser.c -o cmlparser

main: main.c cmlparser.c pipe.c shell.h 
	gcc -Wall -g -v -o main main.c cmlparser.c pipe.c

clean:
	rm -f *.o
