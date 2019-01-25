#include "AsyncGetline.h"

using namespace std;

//This code works perfectly well on Windows 10 in Visual Studio 2015 c++ Win32 Console Debug and Release mode.
//If it doesn't work in your OS or environment, that's too bad; guess you'll have to fix it. :(
//You are free to use this code however you please, with one exception: no plagiarism!
//(You can include this in a much bigger project without giving any credit.)
//Created 02-15-17 by Andrew Davis, the creator of a new programming language called Basik.
//If you like this code, please check it out, thanks! http://thecodingwebsite.com


	//AsyncGetline is a class that allows for asynchronous CLI getline-style input
	//(with 0% CPU usage!), which normal iostream usage does not easily allow.
AsyncGetline::AsyncGetline() {
	input = "";
	sendOverNextLine = true;
	continueGettingInput = true;

	//Start a new detached thread to call getline over and over again and retrieve new input to be processed.
	thread([&]()
	{
		//Non-synchronized string of input for the getline calls.
		string synchronousInput;
		char nextCharacter;

		//Get the asynchronous input lines.
		do
		{
			//Start with an empty line.
			synchronousInput = "";

			//Process input characters one at a time asynchronously, until a new line character is reached.
			while (continueGettingInput)
			{
				//See if there are any input characters available (asynchronously).
				while (cin.peek() == EOF)
				{
					//Ensure that the other thread is always yielded to when necessary. Don't sleep here;
					//only yield, in order to ensure that processing will be as responsive as possible.
					this_thread::yield();
				}

				//Get the next character that is known to be available.
				nextCharacter = cin.get();

				//Check for new line character.
				if (nextCharacter == '\n')
				{
					break;
				}

				//Since this character is not a new line character, add it to the synchronousInput string.
				synchronousInput += nextCharacter;
			}

			//Be ready to stop retrieving input at any moment.
			if (!continueGettingInput)
			{
				break;
			}

			//Wait until the processing thread is ready to process the next line.
			while (continueGettingInput && !sendOverNextLine)
			{
				//Ensure that the other thread is always yielded to when necessary. Don't sleep here;
				//only yield, in order to ensure that the processing will be as responsive as possible.
				this_thread::yield();
			}

			//Be ready to stop retrieving input at any moment.
			if (!continueGettingInput)
			{
				break;
			}

			//Safely send the next line of input over for usage in the processing thread.
			inputLock.lock();
			input = synchronousInput;
			inputLock.unlock();

			//Signal that although this thread will read in the next line,
			//it will not send it over until the processing thread is ready.
			sendOverNextLine = false;
		} while (continueGettingInput && input != "exit");
	}).detach();
}

	//Stop getting asynchronous CLI input.
AsyncGetline::~AsyncGetline() {
	//Stop the getline thread.
	continueGettingInput = false;
}

	//Get the next line of input if there is any; if not, sleep for a millisecond and return an empty string.
string AsyncGetline::GetLine() {
	//See if the next line of input, if any, is ready to be processed.
	if (sendOverNextLine)
	{
		//Don't consume the CPU while waiting for input; this_thread::yield()
		//would still consume a lot of CPU, so sleep must be used.
		this_thread::sleep_for(chrono::milliseconds(1));

		return "";
	}
	else
	{
		//Retrieve the next line of input from the getline thread and store it for return.
		inputLock.lock();
		string returnInput = input;
		inputLock.unlock();

		//Also, signal to the getline thread that it can continue
		//sending over the next line of input, if available.
		sendOverNextLine = true;

		return returnInput;
	}
}
