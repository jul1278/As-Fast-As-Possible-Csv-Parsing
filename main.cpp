#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <vector>
#include <chrono>
#include <ctime>
#include <algorithm>

std::string filepath = "EUR_JPY_Week2.csv";

struct DateTimePricePair {
	unsigned short year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	unsigned int millisec;

	unsigned int quote; 
};


std::vector<DateTimePricePair> dateTimePricePairs;

unsigned int numHeaders = 6;
const unsigned int bufferSize = 8 * 2048;


//----------------------------------------------------------------------------------------------------------
// Name: StreamReadLineByLine
// Desc:
//----------------------------------------------------------------------------------------------------------
void StreamReadLineByLine()
{
	std::ifstream filestream(filepath);

	if (!filestream.is_open()) {

		std::cout << "Error opening file.";
		return;
	}

	dateTimePricePairs.clear();

	while (filestream) {

		std::string row;

		std::getline(filestream, row);
		std::getline(filestream, row); // discard header

		std::stringstream rowStream(row);

		std::string cell;

		unsigned int colCounter = 0;

		DateTimePricePair dateTimePricePair;

		while (std::getline(rowStream, cell, ',')) {

			if (colCounter == 3) {

				unsigned int year;
				unsigned int month;
				unsigned int day;
				unsigned int hour;
				unsigned int minute;
				unsigned int second;
				unsigned int millisec;
#ifdef __WIN32
				sscanf_s(cell.c_str(), "%d-%d-%d %d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &millisec);
#else
				sscanf(cell.c_str(), "%d-%d-%d %d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &millisec);
#endif
				dateTimePricePair.year = year;
				dateTimePricePair.month = month;
				dateTimePricePair.day = day;
				dateTimePricePair.hour = hour;
				dateTimePricePair.minute = minute;
				dateTimePricePair.second = second;
				dateTimePricePair.millisec = millisec;

			}
			else if (colCounter == 4) {

				auto i = cell.find('.');
				cell.replace(i, 1, "");

				dateTimePricePair.quote = std::atoi(cell.c_str());
			}

			colCounter++;
		}

		dateTimePricePairs.push_back(dateTimePricePair);
	}

	filestream.close();
}

//----------------------------------------------------------------------------------------------------------
// Name: StreamReadBlock
// Desc:
//----------------------------------------------------------------------------------------------------------
void StreamReadBlock() {

	std::ifstream filestream(filepath);

	if (!filestream.is_open()) {
		
		std::cout << "Error opening file."; 
		return;
	}

	dateTimePricePairs.clear(); 

	char buffer[bufferSize];
	memset(buffer, 0, sizeof(buffer));

	// We want to ignore the headers so discard characters until the first newline
	unsigned int offset = 0;
	while (filestream.get() != '\n') {
		offset++;
	}

	unsigned int bufferOffset = 0; 
	unsigned int headerCounter = 0; 

	DateTimePricePair dateTimePricePair;
	
	while (filestream) {

		filestream.read(&buffer[bufferOffset], sizeof(buffer) - bufferOffset);
		unsigned int lastCommaIndex = 0; 

		for (auto i = 0; i < sizeof(buffer); i++) {

			// read until we find a ',' then insert a null terminator
			if (buffer[i] == ',' || buffer[i] == '\n') {
				
				char lastChar = buffer[i];
				buffer[i] = 0; 

				if (headerCounter == 3) {

					unsigned int year;
					unsigned int month;
					unsigned int day;
					unsigned int hour;
					unsigned int minute;
					unsigned int second;
					unsigned int millisec;

					// datetime
#ifdef __WIN32
					sscanf_s(&buffer[lastCommaIndex], "%d-%d-%d %d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &millisec); 
#else
					sscanf(&buffer[lastCommaIndex], "%d-%d-%d %d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &millisec); 
#endif
					dateTimePricePair.year = year;
					dateTimePricePair.month = month;
					dateTimePricePair.day = day;
					dateTimePricePair.hour = hour;
					dateTimePricePair.minute = minute;
					dateTimePricePair.second = second;
					dateTimePricePair.millisec = millisec;

				} else if (headerCounter == 4) {

					// do this to avoid constructing an std::string
					char bidStrBuffer[32];
					unsigned int counter = lastCommaIndex; 
					unsigned int copyCounter = 0; 

					while (buffer[counter]) {
						if (buffer[counter] != '.') {
							bidStrBuffer[copyCounter] = buffer[counter]; 
							copyCounter++; 
						}

						counter++; 
					}

					bidStrBuffer[copyCounter] = 0; 

					dateTimePricePair.quote = std::atoi(bidStrBuffer);
					dateTimePricePairs.push_back(dateTimePricePair);
				}

				if (headerCounter >= (numHeaders - 1) || lastChar == '\n') {
					headerCounter = 0;

				} else {

					headerCounter++;
				}

				lastCommaIndex = i + 1; // plus one because we want the element after
			}
			// we're at the last element
			else {
				
				if (i == sizeof(buffer) - 1) {

					// copy the remaining characters back to the beginning of the buffer
					memcpy(buffer, &buffer[lastCommaIndex], i - lastCommaIndex + 1);
					bufferOffset = i - lastCommaIndex + 1; 

					lastCommaIndex = 0; 
				}
			}

		}
	}

	filestream.close();
}

//---------------------------------------------------------------------------------------------------------------------
// Name: main
// Desc:
//---------------------------------------------------------------------------------------------------------------------
int main(int argc, char** argv) {

	if (argc < 2) {
		std::cout << "First argument must be path to test file.\n";
		return 0;
	}

	filepath = argv[1]; 

	std::cout << filepath << std::endl; 

	std::chrono::time_point<std::chrono::system_clock> start; 
	std::chrono::time_point<std::chrono::system_clock> end;

	start = std::chrono::system_clock::now();
	StreamReadLineByLine(); // 25.6 seconds roughly and about 16 megabytes in vstudio debugger
	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsedSeconds = end - start; 
	std::cout << "Line by line stream: " << elapsedSeconds.count() << std::endl; 

	start = std::chrono::system_clock::now();
	StreamReadBlock(); // 2.4 seconds in visual studio debugger
	end = std::chrono::system_clock::now();

	elapsedSeconds = end - start; 
	std::cout << "Read Block: " << elapsedSeconds.count() << std::endl; 

	return 0;
}