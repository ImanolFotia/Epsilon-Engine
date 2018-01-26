#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "25";
	static const char MONTH[] = "01";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.01";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 9;
	static const long MINOR  = 8;
	static const long BUILD  = 9625;
	static const long REVISION  = 52443;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 28157;
	#define RC_FILEVERSION 9,8,9625,52443
	#define RC_FILEVERSION_STRING "9, 8, 9625, 52443\0"
	static const char FULLVERSION_STRING [] = "9.8.9625.52443";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 25;
	

}
#endif //VERSION_H
