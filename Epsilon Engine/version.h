#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "20";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 9;
	static const long MINOR  = 0;
	static const long BUILD  = 8824;
	static const long REVISION  = 48077;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 25457;
	#define RC_FILEVERSION 9,0,8824,48077
	#define RC_FILEVERSION_STRING "9, 0, 8824, 48077\0"
	static const char FULLVERSION_STRING [] = "9.0.8824.48077";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 24;
	

}
#endif //VERSION_H
