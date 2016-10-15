#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "15";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.10";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 6;
	static const long MINOR  = 4;
	static const long BUILD  = 5979;
	static const long REVISION  = 32381;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 16371;
	#define RC_FILEVERSION 6,4,5979,32381
	#define RC_FILEVERSION_STRING "6, 4, 5979, 32381\0"
	static const char FULLVERSION_STRING [] = "6.4.5979.32381";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 79;
	

}
#endif //VERSION_H
