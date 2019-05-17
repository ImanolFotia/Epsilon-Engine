#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "16";
	static const char MONTH[] = "05";
	static const char YEAR[] = "2019";
	static const char UBUNTU_VERSION_STYLE[] =  "19.05";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 11;
	static const long MINOR  = 8;
	static const long BUILD  = 11804;
	static const long REVISION  = 64467;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 34377;
	#define RC_FILEVERSION 11,8,11804,64467
	#define RC_FILEVERSION_STRING "11, 8, 11804, 64467\0"
	static const char FULLVERSION_STRING [] = "11.8.11804.64467";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 4;
	

}
#endif //VERSION_H
