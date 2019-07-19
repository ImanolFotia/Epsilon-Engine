#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "29";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2019";
	static const char UBUNTU_VERSION_STYLE[] =  "19.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 9;
	static const long BUILD  = 11909;
	static const long REVISION  = 65078;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 34732;
	#define RC_FILEVERSION 11,9,11909,65078
	#define RC_FILEVERSION_STRING "11, 9, 11909, 65078\0"
	static const char FULLVERSION_STRING [] = "11.9.11909.65078";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 9;
	

}
#endif //VERSION_H
