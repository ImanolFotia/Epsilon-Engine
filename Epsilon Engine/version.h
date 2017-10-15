#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "22";
	static const char MONTH[] = "08";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.08";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 9;
	static const long MINOR  = 0;
	static const long BUILD  = 8868;
	static const long REVISION  = 48309;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 25570;
	#define RC_FILEVERSION 9,0,8868,48309
	#define RC_FILEVERSION_STRING "9, 0, 8868, 48309\0"
	static const char FULLVERSION_STRING [] = "9.0.8868.48309";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 68;
	

}
#endif //VERSION_H
