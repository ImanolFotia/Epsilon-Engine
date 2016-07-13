#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "13";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.07";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 5;
	static const long MINOR  = 1;
	static const long BUILD  = 4504;
	static const long REVISION  = 24445;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 11046;
	#define RC_FILEVERSION 5,1,4504,24445
	#define RC_FILEVERSION_STRING "5, 1, 4504, 24445\0"
	static const char FULLVERSION_STRING [] = "5.1.4504.24445";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 4;
	

}
#endif //VERSION_H
