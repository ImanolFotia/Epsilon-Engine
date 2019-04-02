#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "02";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2019";
	static const char UBUNTU_VERSION_STYLE[] =  "19.04";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 6;
	static const long BUILD  = 11678;
	static const long REVISION  = 63827;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 33831;
	#define RC_FILEVERSION 11,6,11678,63827
	#define RC_FILEVERSION_STRING "11, 6, 11678, 63827\0"
	static const char FULLVERSION_STRING [] = "11.6.11678.63827";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 78;
	

}
#endif //VERSION_H
