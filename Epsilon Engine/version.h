#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "18";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.03";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 2;
	static const long MINOR  = 3;
	static const long BUILD  = 1496;
	static const long REVISION  = 8240;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 3268;
	#define RC_FILEVERSION 2,3,1496,8240
	#define RC_FILEVERSION_STRING "2, 3, 1496, 8240\0"
	static const char FULLVERSION_STRING [] = "2.3.1496.8240";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 96;
	

}
#endif //VERSION_H
