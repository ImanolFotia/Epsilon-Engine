#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "10";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 4;
	static const long MINOR  = 1;
	static const long BUILD  = 3431;
	static const long REVISION  = 18638;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 7739;
	#define RC_FILEVERSION 4,1,3431,18638
	#define RC_FILEVERSION_STRING "4, 1, 3431, 18638\0"
	static const char FULLVERSION_STRING [] = "4.1.3431.18638";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 31;
	

}
#endif //VERSION_H
