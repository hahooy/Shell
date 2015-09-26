# Yongzheng Huang, 445575 Operating Systems Project 1

sish: sish.c
	gcc -Wall -g sish.c -o sish

exp: exp.c
	gcc -Wall -g exp.c -o exp

cmlparser: cmlparser.c
	gcc -Wall -g cmlparser.c -o cmlparser

clean:
	rm -f sish exp
