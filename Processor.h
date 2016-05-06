/*
 * Processor.h
 *
 *  Created on: Sep 26, 2014
 *      Author: Lawrence Hsia
 */

#ifndef PROCESSOR_H_
#include "flags.h"

#include <cstdlib>
#include <ctime>
#include <stdlib.h>
#include <unistd.h>

class Processor
{
public:
	//this is so executing instructions can be done with a switch statement
	enum Instruction
	{
		LoadValue = 1,  //directly load value into AC
		LoadAddr , //load value found in address into AC
		LoadInd_addr , //load indirectly into AC. If address holds another address, call it address B, load value inside address B
		LoadIdxX_addr , //load the value at (address + X) into AC
		LoadIdxY_addr , //load value at (address + Y) into AC
		LoadSpX ,
		StoreAddress  ,//put AC's value into the address
		Get ,
		PutPort ,
		AddX ,
		AddY ,
		SubX ,
		SubY ,
		CopyToX ,
		CopyFromX ,
		CopyToY ,
		CopyFromY ,
		CopyToSP ,
		CopyFromSP ,
		JumpAddress	,
		JumpIfEqual_Addr , //jump to addr if (AC == 0)
		JumpIfNotEqual_Addr, //jump to addr if(AC != 0)
		CallAddr, //user will provide address to jump to. pushes PC's address # onto stack,
		Ret, //pop return address from stack
		IncX , //X++
		DecX , // Y--
		Push, //push AC onto stack
		Pop, //pops AC off stack
		Int,
		IRet,
		End = 50
	};
	 //need to be modified and access outside of class often
	//so they're public
	int PC;

public:
	Processor(int& r, int& w);
	void executeInstruction();
	void setIR(int instruction);
	void setPipeEnds(int r, int w);
	int readFromMemory(int address);
	void writeToAddress(int data, int address);
	void handleInterrupt(int intType);
	void interruptReturn();
	void checkInterrupt(int& counter, int& timer);
	//fetch instruction done via pipe

private:
	int  loadAddress(int loopTimes);
	int  loadIndexAddress(int x);
	int  popFromStack();
	void pushToStack(int&);
	void jumpToAddress();

private:
	int SP, IR, AC, X, Y;
	int mode;
	int readEnd;
	int writeEnd;
};


#endif /* PROCESSOR_H_ */
