#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "13";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 9;
	static const long MINOR  = 3;
	static const long BUILD  = 9195;
	static const long REVISION  = 50165;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 26478;
	#define RC_FILEVERSION 9,3,9195,50165
	#define RC_FILEVERSION_STRING "9, 3, 9195, 50165\0"
	static const char FULLVERSION_STRING [] = "9.3.9195.50165";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 95;
	

}
#endif //VERSION_H
