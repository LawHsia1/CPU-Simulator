#include "Computer.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int main(int argc, char *argv[])
{
	int timer = 0;
	char *fileName = "sample 2.txt";
	//cout << argc << endl;
	if(argc > 1)
	{
		fileName = argv[1];
		timer = atoi(argv[2]);
	}

	Computer computer(timer, fileName);
	computer.run();
}
