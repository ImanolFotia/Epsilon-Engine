#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "12";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.01";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 7;
	static const long MINOR  = 6;
	static const long BUILD  = 7211;
	static const long REVISION  = 39227;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 20034;
	#define RC_FILEVERSION 7,6,7211,39227
	#define RC_FILEVERSION_STRING "7, 6, 7211, 39227\0"
	static const char FULLVERSION_STRING [] = "7.6.7211.39227";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 11;
	

}
#endif //VERSION_H
