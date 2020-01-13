#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "08";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2020";
	static const char UBUNTU_VERSION_STYLE[] =  "20.01";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 9;
	static const long BUILD  = 11973;
	static const long REVISION  = 65427;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 35030;
	#define RC_FILEVERSION 11,9,11973,65427
	#define RC_FILEVERSION_STRING "11, 9, 11973, 65427\0"
	static const char FULLVERSION_STRING [] = "11.9.11973.65427";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 73;
	

}
#endif //VERSION_H
