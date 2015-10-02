#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int seconds = 10;
    if (argc > 1) {
	seconds = atoi(argv[1]);
    }
    printf("start sleep for %d seconds\n", seconds);
    sleep(seconds);
    printf("end sleep\n");
}
