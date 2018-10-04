#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "04";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.10";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 4;
	static const long BUILD  = 11484;
	static const long REVISION  = 62760;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 33253;
	#define RC_FILEVERSION 11,4,11484,62760
	#define RC_FILEVERSION_STRING "11, 4, 11484, 62760\0"
	static const char FULLVERSION_STRING [] = "11.4.11484.62760";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 84;
	

}
#endif //VERSION_H
