#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "18";
	static const char MONTH[] = "03";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.03";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 8;
	static const long MINOR  = 3;
	static const long BUILD  = 8029;
	static const long REVISION  = 43744;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 23371;
	#define RC_FILEVERSION 8,3,8029,43744
	#define RC_FILEVERSION_STRING "8, 3, 8029, 43744\0"
	static const char FULLVERSION_STRING [] = "8.3.8029.43744";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 29;
	

}
#endif //VERSION_H
