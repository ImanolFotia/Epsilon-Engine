#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "06";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2019";
	static const char UBUNTU_VERSION_STYLE[] =  "19.03";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 6;
	static const long BUILD  = 11618;
	static const long REVISION  = 63459;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 33690;
	#define RC_FILEVERSION 11,6,11618,63459
	#define RC_FILEVERSION_STRING "11, 6, 11618, 63459\0"
	static const char FULLVERSION_STRING [] = "11.6.11618.63459";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 18;
	

}
#endif //VERSION_H
