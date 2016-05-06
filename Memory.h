/*
 * Memory.h
 *
 *  Created on: Sep 26, 2014
 *      Author: rayhsia
 */

#ifndef MEMORY_H_
#define MEMORY_H_
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "flags.h"

#include <stdlib.h>
#include <unistd.h>


class Memory
{
public:
	enum ErrorNums
	{
		INVALID_ACCESS = 1,
		BOUNDS_ERROR,
	};

public:
	Memory();
	Memory(char* fileName);
	int readAddress(int, int);
	int readFromCPU(); //function returns two values. The value obtained from CPU and the flag obtained
	void writeToCPU(int valueToWrite);
	void writeToAddress(int data, int address, int mode);
	void setPipeEnds(int r, int w);
	~Memory();

private:
	int stringToNum(std::string&);

private:
	int cells[2000];
	int readEnd;
	int writeEnd;
	bool sysMode;
	// 0 - 999 = user programs
	// 1000 - 1999 = system code
};

#endif /* MEMORY_H_ */
