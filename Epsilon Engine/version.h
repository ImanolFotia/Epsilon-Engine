#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "25";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.05";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 8;
	static const long MINOR  = 8;
	static const long BUILD  = 8596;
	static const long REVISION  = 46825;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 25005;
	#define RC_FILEVERSION 8,8,8596,46825
	#define RC_FILEVERSION_STRING "8, 8, 8596, 46825\0"
	static const char FULLVERSION_STRING [] = "8.8.8596.46825";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 96;
	

}
#endif //VERSION_H
