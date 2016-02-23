#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "22";
	static const char MONTH[] = "02";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.02";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 1;
	static const long MINOR  = 5;
	static const long BUILD  = 558;
	static const long REVISION  = 3077;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 1423;
	#define RC_FILEVERSION 1,5,558,3077
	#define RC_FILEVERSION_STRING "1, 5, 558, 3077\0"
	static const char FULLVERSION_STRING [] = "1.5.558.3077";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 58;
	

}
#endif //VERSION_H
