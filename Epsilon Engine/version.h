#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "03";
	static const char MONTH[] = "09";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.09";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 10;
	static const long MINOR  = 1;
	static const long BUILD  = 10001;
	static const long REVISION  = 54624;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 29308;
	#define RC_FILEVERSION 10,1,10001,54624
	#define RC_FILEVERSION_STRING "10, 1, 10001, 54624\0"
	static const char FULLVERSION_STRING [] = "10.1.10001.54624";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 1;
	

}
#endif //VERSION_H
