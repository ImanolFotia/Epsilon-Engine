#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "23";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 4;
	static const long MINOR  = 7;
	static const long BUILD  = 4029;
	static const long REVISION  = 21856;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 9860;
	#define RC_FILEVERSION 4,7,4029,21856
	#define RC_FILEVERSION_STRING "4, 7, 4029, 21856\0"
	static const char FULLVERSION_STRING [] = "4.7.4029.21856";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 29;
	

}
#endif //VERSION_H
