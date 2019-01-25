#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <atomic>


class AsyncGetline
{
public:
	//AsyncGetline is a class that allows for asynchronous CLI getline-style input
	//(with 0% CPU usage!), which normal iostream usage does not easily allow.
	AsyncGetline();

	//Stop getting asynchronous CLI input.
	~AsyncGetline();

	//Get the next line of input if there is any; if not, sleep for a millisecond and return an empty string.
	std::string GetLine();

private:
	//Cross-thread-safe boolean to tell the getline thread to stop when AsyncGetline is deconstructed.
	std::atomic<bool> continueGettingInput;

	//Cross-thread-safe boolean to denote when the processing thread is ready for the next input line.
	//This exists to prevent any previous line(s) from being overwritten by new input lines without
	//using a queue by only processing further getline input when the processing thread is ready.
	std::atomic<bool> sendOverNextLine;

	//Mutex lock to ensure only one thread (processing vs. getline) is accessing the input string at a time.
	std::mutex inputLock;

	//string utilized safely by each thread due to the inputLock mutex.
	std::string input;
};