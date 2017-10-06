#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "02";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.10";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 10;
	static const long MINOR  = 4;
	static const long BUILD  = 10300;
	static const long REVISION  = 56267;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 30321;
	#define RC_FILEVERSION 10,4,10300,56267
	#define RC_FILEVERSION_STRING "10, 4, 10300, 56267\0"
	static const char FULLVERSION_STRING [] = "10.4.10300.56267";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 0;
	

}
#endif //VERSION_H
