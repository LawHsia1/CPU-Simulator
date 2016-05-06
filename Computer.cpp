/*
 * Computer.cpp
 *
 *  Created on: Sep 28, 2014
 *      Author: rayhsia
 */

#include "Computer.h"

Computer::Computer(int& t, char *fileName):
 timer(t)
, mainMemory(fileName)
{}

void Computer::run()
{
	using namespace std;

	//Memory mainMemory("sample 4.txt");//file name also taken in through argv
	//timer = 0;//TODO: have this value taken in through argv
	int instruction = 0;
	int rubbish = -1;//junk variable for CPU to send to end memory process


	int result;
	result = pipe(CPUBus);//create CPU's sending pipe
	checkResult(result);

	result = pipe(memoryBus);//create memory's sending pipe
	checkResult(result);

	result = fork(); //fork the process
	checkResult(result);

	switch(result)
	{
		default://parent case. Parent is CPU
		{
			close(CPUBus[0]);
			close(memoryBus[1]);
			Processor CPU(memoryBus[0], CPUBus[1]);//initialize CPU with pipe endings

			int instrCounter = 0;
			while(true)
			{
				instruction = CPU.readFromMemory(CPU.PC);
				if(instruction == 50)
					break;

				CPU.setIR(instruction);
				CPU.executeInstruction();

				if(timer > 0)
					CPU.checkInterrupt(instrCounter, timer);
			}
			write(CPUBus[1], &rubbish, sizeof(int));
			write(CPUBus[1], &rubbish, sizeof(int)); //send negative flag to terminate memory process

			waitpid(0, NULL, -1); //wait for memory to exit
			break;
		}//braces here so I can make temp variables inside my switch cases

		case 0: //child case. Child process = memory
			close(CPUBus[1]);
			close(memoryBus[0]);
			mainMemory.setPipeEnds(CPUBus[0], memoryBus[1]);

			int address = 0;
			int flag = 0;
			int mode = 0;

			//loop part
			while(true)
			{
				address = mainMemory.readFromCPU();
				flag = mainMemory.readFromCPU();
				mode = mainMemory.readFromCPU();

				if(flag == readAddr)
					mainMemory.writeToCPU( mainMemory.readAddress(address, mode) );
				else if(flag == writeAddr)
				{
					int data;
					data = mainMemory.readFromCPU();//puts result into data
					mainMemory.writeToAddress(data, address, mode);
				}
				else if(flag < 0)
					break;
			}

			exit(0);
	}

}

void Computer::checkResult(int& result)
{
	if(result == -1)
		std::cerr << "pipe/fork failed" << std::endl;
}

Computer::~Computer()
{

}
