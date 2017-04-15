#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "15";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.04";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 8;
	static const long MINOR  = 4;
	static const long BUILD  = 8135;
	static const long REVISION  = 44311;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 23943;
	#define RC_FILEVERSION 8,4,8135,44311
	#define RC_FILEVERSION_STRING "8, 4, 8135, 44311\0"
	static const char FULLVERSION_STRING [] = "8.4.8135.44311";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 35;
	

}
#endif //VERSION_H
