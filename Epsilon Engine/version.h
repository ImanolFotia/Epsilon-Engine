#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "11";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.03";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 8;
	static const long MINOR  = 1;
	static const long BUILD  = 7867;
	static const long REVISION  = 42885;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 22760;
	#define RC_FILEVERSION 8,1,7867,42885
	#define RC_FILEVERSION_STRING "8, 1, 7867, 42885\0"
	static const char FULLVERSION_STRING [] = "8.1.7867.42885";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 67;
	

}
#endif //VERSION_H
