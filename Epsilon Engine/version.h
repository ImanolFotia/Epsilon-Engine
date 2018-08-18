#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "17";
	static const char MONTH[] = "08";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.08";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 2;
	static const long BUILD  = 11269;
	static const long REVISION  = 61687;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 32501;
	#define RC_FILEVERSION 11,2,11269,61687
	#define RC_FILEVERSION_STRING "11, 2, 11269, 61687\0"
	static const char FULLVERSION_STRING [] = "11.2.11269.61687";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 69;
	

}
#endif //VERSION_H
