# Junyuan Suo, ID: 446842, email: j.suo@wustl.edu
# Yilong Hu, ID: 438033, email: hu.yilong@wustl.edu
# Yongzheng Huang, ID: 445575, email: huangyongzheng@wustl.edu

# Operating Systems Project 1.
# To compile the sish source code, type "make sish" on the command line.
# To run the sish shell, type "./sish".

sish: main.c cmlparser.c pipe.c my_signo.c buildin.c supplement.c shell.h 
	gcc -std=gnu11 -Wall -g -o sish main.c cmlparser.c pipe.c my_signo.c buildin.c supplement.c

clean:
	rm -f *.o
