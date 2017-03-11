#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "10";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.03";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 8;
	static const long MINOR  = 1;
	static const long BUILD  = 7850;
	static const long REVISION  = 42787;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 22630;
	#define RC_FILEVERSION 8,1,7850,42787
	#define RC_FILEVERSION_STRING "8, 1, 7850, 42787\0"
	static const char FULLVERSION_STRING [] = "8.1.7850.42787";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 50;
	

}
#endif //VERSION_H
