#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <vector>
#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <algorithm>

struct DateTimePricePair {
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int millisec;

	unsigned int quote; 
};

//----------------------------------------------------------------------------------------------------------
// Name: StreamReadLineByLine
// Desc:
//----------------------------------------------------------------------------------------------------------
void StreamReadLineByLine()
{
	std::string filepath = "C:\\Julian\\Prices\\2015\\USD_JPY\\January\\USD_JPY_Week1.csv";
	std::ifstream filestream(filepath);

	std::vector<DateTimePricePair> dateTimePricePairs;

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

				sscanf_s(cell.c_str(), "%d-%d-%d %d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &millisec);

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
	std::string filepath = "C:\\Julian\\Prices\\2015\\USD_JPY\\January\\USD_JPY_Week1.csv";
	std::ifstream filestream(filepath);

	std::vector<DateTimePricePair> dateTimePricePairs;

	unsigned int numHeaders = 6; 

	const unsigned int bufferSize = 8*2048;

	char buffer[bufferSize];
	memset(buffer, 0, sizeof(buffer));

	// find offset
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
					sscanf_s(&buffer[lastCommaIndex], "%d-%d-%d %d:%d:%d.%d", &year, &month, &day, &hour, &minute, &second, &millisec); 

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

int main(int argc, char** argv) {

	//StreamReadLineByLine(); // 17 seconds roughly and about 10 megabytes

	StreamReadBlock(); // 2.76 seconds

	return 0;

}