# Junyuan Suo,
# Yilong Hu,
# Yongzheng Huang, 445575
# Operating Systems Project 1.
# To compile the sish source code, type "make main" on the command line.
# To run the sish shell, type "./sish".

sish: sish.c
	gcc -Wall -g sish.c -o sish

exp: exp.c
	gcc -Wall -g exp.c -o exp

cmlparser: cmlparser.c shell.h
	gcc -Wall -g cmlparser.c -o cmlparser

main: main.c cmlparser.c pipe.c my_signo.c buildin.c supplement.c shell.h 
	gcc -Wall -g -o sish main.c cmlparser.c pipe.c my_signo.c buildin.c supplement.c

clean:
	rm -f *.o
