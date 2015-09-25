# Yongzheng Huang, 445575 Operating Systems Project 1

sish: sish.c
	gcc -Wall -g sish.c -o sish

exp: exp.c
	gcc -Wall -g exp.c -o exp

clean:
	rm -f sish exp
