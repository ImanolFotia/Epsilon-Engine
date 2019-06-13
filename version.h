#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "10";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2019";
	static const char UBUNTU_VERSION_STYLE[] =  "19.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 8;
	static const long BUILD  = 11887;
	static const long REVISION  = 64937;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 34663;
	#define RC_FILEVERSION 11,8,11887,64937
	#define RC_FILEVERSION_STRING "11, 8, 11887, 64937\0"
	static const char FULLVERSION_STRING [] = "11.8.11887.64937";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 87;
	

}
#endif //VERSION_H
