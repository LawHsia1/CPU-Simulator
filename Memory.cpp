/*
 *  Memory.cpp
 *  ProcessorMemory
 *
 *  Created by Ray Hsia on 9/18/14.
 *  Copyright 2014 __MyCompanyName__. All rights reserved.
 *
 */

#include "Memory.h"

Memory::Memory()
{
	//placeholder initialization
	for(int i = 0; i < 2000; i++)
		cells[i] = i;
}

//initializes memory cells w/ text file
Memory::Memory(char* fileName)
: readEnd(0)
, writeEnd(0)
{
	using namespace std;
	ifstream inputFile(fileName);
	string temp;

	for(int index = 0;inputFile >> temp; index++)
	{
		int result;
		if(temp.find(".") != string::npos)
		{
			temp.erase(temp.begin());//remove the first character, the period
			result = stringToNum(temp);//change temp -> number
			index = result;//change loading position
			inputFile >> temp; //get next # in text file
		}
		result = stringToNum(temp);
		cells[index] = result;
	}
}

//reads data from an address
int Memory::readAddress(int address, int mode)
{
	if(mode == userMode && address > 999)
		throw std::out_of_range("user mode cannot access addresses over 999");
	else
		return cells[address]; //returns the value @ the index
}

//can read in address or flag
int Memory::readFromCPU()
{
	int value;
	read(readEnd, &value, sizeof(int)); //reads in a value
	//std::cout << "Memory read in the value: " << value << std::endl;

	return value;
}

void Memory::writeToCPU(int valueToWrite)
{
	write(writeEnd, &valueToWrite, sizeof(int));//write what memory got back over to processor process
}

//writes data to an address
//assumes user mode for now.
void Memory::writeToAddress( int data, int address, int mode )
{
	if(mode == userMode && address > 999)
		throw std::out_of_range("user mode cannot access addresses over 999");
	else
		cells[address] = data;
}

void Memory::setPipeEnds(int r, int w)
{
	readEnd = r;
	writeEnd = w;
}

int Memory::stringToNum(std::string& numText)
{
	int result;
	std::stringstream convert(numText);
	convert >> result;
	return result;
}

Memory::~Memory()
{

}
