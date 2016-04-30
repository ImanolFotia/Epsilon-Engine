#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "29";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.04";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 3;
	static const long MINOR  = 4;
	static const long BUILD  = 2605;
	static const long REVISION  = 14135;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 6169;
	#define RC_FILEVERSION 3,4,2605,14135
	#define RC_FILEVERSION_STRING "3, 4, 2605, 14135\0"
	static const char FULLVERSION_STRING [] = "3.4.2605.14135";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 5;
	

}
#endif //VERSION_H
