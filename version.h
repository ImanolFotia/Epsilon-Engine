#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "26";
	static const char MONTH[] = "11";
	static const char YEAR[] = "2019";
	static const char UBUNTU_VERSION_STYLE[] =  "19.11";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 9;
	static const long BUILD  = 11943;
	static const long REVISION  = 65263;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 34909;
	#define RC_FILEVERSION 11,9,11943,65263
	#define RC_FILEVERSION_STRING "11, 9, 11943, 65263\0"
	static const char FULLVERSION_STRING [] = "11.9.11943.65263";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 43;
	

}
#endif //VERSION_H
