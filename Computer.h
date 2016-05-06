/*
 * Computer.h
 *
 *  Created on: Sep 28, 2014
 *      Author: rayhsia
 */

#ifndef COMPUTER_H_
#define COMPUTER_H_

#include "Processor.h"
#include "Memory.h"
#include "flags.h"

#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>

class Computer {
public:
	Computer(int&, char*);
	void run();
	virtual ~Computer();

private:
	void checkResult(int&);

private:
	int timer;
	int CPUBus[2];
	int memoryBus[2];
	Memory mainMemory;

};

#endif /* COMPUTER_H_ */
