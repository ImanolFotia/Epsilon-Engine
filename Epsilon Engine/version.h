#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "03";
	static const char MONTH[] = "09";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.09";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 3;
	static const long BUILD  = 11360;
	static const long REVISION  = 62109;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 32786;
	#define RC_FILEVERSION 11,3,11360,62109
	#define RC_FILEVERSION_STRING "11, 3, 11360, 62109\0"
	static const char FULLVERSION_STRING [] = "11.3.11360.62109";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 60;
	

}
#endif //VERSION_H
