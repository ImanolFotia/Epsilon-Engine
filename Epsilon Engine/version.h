#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "01";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 10;
	static const long MINOR  = 9;
	static const long BUILD  = 10813;
	static const long REVISION  = 59159;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 32481;
	#define RC_FILEVERSION 10,9,10813,59159
	#define RC_FILEVERSION_STRING "10, 9, 10813, 59159\0"
	static const char FULLVERSION_STRING [] = "10.9.10813.59159";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 13;
	

}
#endif //VERSION_H
