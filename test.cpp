#include <iostream>
#include <map>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

using namespace std;

typedef void (*fp)(int[]);

map<string, fp> create_map();
void func1(int *);
void func2(int *);


int main(){
	map<string, fp> mp = create_map();
	int arr1[] = {4}, arr2[] = {5};

	mp["func1"](arr1);
	mp["func2"](arr2);
	return 0;
}

map<string, fp> create_map()
{
	map<string, fp> mp;
	// m["show"] = show();
	// m["set"] = ;
	// m["unset"] = ;
	// m["export"] = ;
	// m["unexport"] = ;
	// m["environ"] = ;
	// m["chdir"] = ;
	// m["exit"] = ;
	// m["wait"] = ;
	// m["clr"] = ;
	// m["dir"] = ;
	// m["echo"] = ;
	// m["help"] = ;
	// m["pause"] = ;
	// m["history"] = ;
	// m["repeat"] = ;
	// m["kill"] = ;
	mp["func1"] = func1;
	mp["func2"] = func2;
	return mp;
}

void func1(int *x){
	cout << "hello world!" << endl;
	cout << "x = " << x[0] << endl;
}

void func2(int *x){
	cout << "trying" << endl;
	cout << "x = " << x[0] << endl;
}