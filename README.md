# As Fast As Possible Csv Parsing
Trying to parse reasonably large CSV files in C++ as fast as I can.

The test file is a weeks worth of forex tick data with about 660k rows. 
We're reading in the data and parsing out the date/time and bid price.

The following measurements are taken from visual studio debug mode which displays the time in ms between breakpoints
so relatively rudimentary measurements. 

my machine has the following specs: i7-7700k @ 4.2GHz, 32GB RAM @ 1064MHz

reading line by line from fstream we get about 25 seconds, 
however a reasonable amount of that is possibly string::find() string::replace() 

reading by block from a stream we get down to about 2.5 seconds and we're not constructing any std::strings etc
just using char array

up next is to try ```mmap``` and whatever the windows equivalent is for memory mapped files

and also asynchronous reading/processing
