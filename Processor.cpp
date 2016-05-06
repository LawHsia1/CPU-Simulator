/*
 *  Processor.cpp
 *  ProcessorMemory
 *
 *  Created by Ray Hsia on 9/21/14.
 *
 */

#include "Processor.h"
#include <iostream>

Processor::Processor(int& r, int& w)
: PC(0), //program counter
SP(1000), //stack pointer
IR(0), //Instruction reader
AC(0), //accumulator
X(0), //variable x
Y(0), //variable y
mode(userMode),
readEnd(r),
writeEnd(w)
{}

void Processor::executeInstruction()
{
	using namespace std;
	int nextParameter;

	//instruction already loaded. Note to self
	switch (IR)
	{
		case LoadValue:
			PC++; //inc PC to point to next value in memory
			AC = readFromMemory(PC);
			break;

		case LoadAddr:
			AC = loadAddress(2);
			break;

		case LoadInd_addr:
			AC = loadAddress(3);
			break;

		//loads the value at X + address
		case LoadIdxX_addr:
			AC = loadIndexAddress(X);
			break;

		case LoadIdxY_addr:
			AC = loadIndexAddress(Y);
			break;

		case LoadSpX:
			AC = readFromMemory(SP + X);
			break;

		case StoreAddress:
		{//getting the address to write to
			PC++;
			int writeAddress = readFromMemory(PC);

			//writing to the address
			writeToAddress(writeAddress, AC);
			break;
	}
		case Get:
			srand( (unsigned) time(0));
			AC = ( rand() % 100 ) + 1; //generates # from 1 - 100 and shoves it into AC
			break;

		case PutPort:
			PC++;
			nextParameter = readFromMemory(PC);
			switch(nextParameter)
			{
				case 1:
					cout << AC; //writes AC to the screen as an int
					break;
				case 2:
					cout << (char) AC; //writes AC to the screen as a char
					break;
			}
			break;

		//add value in X to AC
		case AddX:
			AC = AC + X;
			break;

		case AddY:
			AC = AC + Y;
			break;

		case SubX:
			AC = AC - X;
			break;

		case SubY:
			AC = AC - Y;
			break;

		case CopyToX:
			X = AC;
			break;

		case CopyFromX:
			AC = X;
			break;

		case CopyToY:
			Y = AC;
			break;

		case CopyFromY:
			AC = Y;
			break;

		case CopyToSP:
			SP = AC;
			break;

		case CopyFromSP:
			AC = SP;
			break;

		case JumpAddress:
			jumpToAddress();
			break;

		case JumpIfEqual_Addr:
			if(AC == 0)
				jumpToAddress();
			else
				PC++;//skip over jump commands parameter
			break;

		case JumpIfNotEqual_Addr:
			if(AC != 0)
				jumpToAddress();
			else
				PC++;//skip over jump commands parameter
			break;

		case CallAddr:
			pushToStack(PC);
			jumpToAddress();
			break;

		case Ret:
			PC = popFromStack() + 1;
			if(PC == -1)
				cout << "Popped from empty stack" << endl;
			/*
			 * Not subtracting one to whatever value is retrieved from memory because
			 * If one is subtracted from PC's value and then readded at the outside of the switch statement
			 * The processor will execute the instruction the PC restored to
			 * We want the processor to move on from this instruction. We want to skip over the instruction
			 * PC got restored to
			 */
			break;

		case IncX:
			X++;
			break;

		case DecX:
			X--;
			break;

		case Push:
			pushToStack(AC);
			break;

		case Pop:
			AC = popFromStack();
			break;

		case Int:
			handleInterrupt(sysCall);
			break;

		case IRet:
			interruptReturn();
			break;

		case End:
			break;

		default:
			cout << "Not coded yet";
			break;
	}

	//PC incremented at the end of execution cycle
	PC++;
}

void Processor::setIR(int instruction)
{
	IR = instruction;
}

void Processor::setPipeEnds(int r, int w)
{
	readEnd = r;
	writeEnd = w;
}

int Processor::readFromMemory(int address)
{
	int result;
	int flag = readAddr;
	write(writeEnd, &address, sizeof(int)); //send address
	write(writeEnd, &flag, sizeof(int));//send reading flag
	write(writeEnd, &mode, sizeof(int));//send processor mode. Could be user mode or system mode

	if(mode == userMode && address > 999)
		exit(-1);

	read(readEnd, &result, sizeof(int));//read what memory sends back
	return result;
}

void Processor::writeToAddress(int address, int data)
{
	int flag = writeAddr;
	write(writeEnd, &address, sizeof(int));//send address first
	write(writeEnd, &flag, sizeof(int));//send the writing flag
	write(writeEnd, &mode, sizeof(int));//send mode
	write(writeEnd, &data, sizeof(int));//send the data last

	if(mode == userMode && address > 999)
		exit(-1);

}

void Processor::handleInterrupt(int intType)
{
	mode = sysMode;
	int oldSP = SP;

	SP = 2000; //switch stacks

	//store registers
	pushToStack(Y);
	pushToStack(X);
	pushToStack(AC);
	pushToStack(IR);
	pushToStack(oldSP);
 	pushToStack(PC);

 	switch(intType)//depending on the interrupt type
 	{
 		case timerInt://timer interrupt handler is at index 1000
 			PC = 1000;//doesn't need to be the address - 1
 			break;

 		case sysCall://syscall interrupt handler is at index 1500
 			PC = 1499;//since PC will be incremented at the end of execute cycle
 			break;
 	}
}

void Processor::interruptReturn()
{
	int userSP;

	if(PC >= 1000 && PC < 1500)//subtract one for timer int mode
		PC = popFromStack() - 1;
	else
		PC = popFromStack();//don't subtract one for syscall

	userSP = popFromStack();
	IR = popFromStack();
	AC = popFromStack();
	X = popFromStack();
	Y = popFromStack();

	SP = userSP;
	mode = userMode;
}

void Processor::checkInterrupt(int& counter, int& timer)
{

	if(mode == userMode )
	{
		if(counter == timer)
		{
			handleInterrupt(timerInt);
			counter = 0;//reset counter
			return;
		}
		else
		{
			counter++;//only increments instruction # when in user mode
			return;
		}
	}

}

//code for loading address and loading indr address because they're both so similiar
int Processor::loadAddress(int loopTimes)
{
	PC++;
	int temp = PC;

	//first loop will get the next memory cell
	//second loop will jump to what the first loop got.
	//looping 3 times will do the loadIndr code
	for(int i = 0; i < loopTimes; i++)
		temp = readFromMemory(temp);

	return temp;
}

int Processor::loadIndexAddress(int x)
{
	PC++;
	int nextParam = readFromMemory(PC);

	return readFromMemory(nextParam + x);
}

int Processor::popFromStack()
{
	if(SP == 1000 || SP == 2000)
		return -1;//return negative # to indicate can't pop anymore

	int top = readFromMemory(SP);//get top element
	SP++;//move stack pointer down
	//std::cout << "popped " << top << " off stack" << std::endl;
	return top;
}

void Processor::pushToStack(int& valueToPush)
{
	SP--;//move the stack pointer up
	writeToAddress(SP, valueToPush);
	//std::cout << "pushed " << valueToPush << " onto stack" << std::endl;
}

void Processor::jumpToAddress()
{
	PC++;
	PC = readFromMemory(PC) - 1;
	//go to the next memory cell and put that value into PC
	//subtracts one to negate the PC++ that comes at the end of the executeInstruction()
}



