#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "21";
	static const char MONTH[] = "11";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.11";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 6;
	static const long MINOR  = 9;
	static const long BUILD  = 6470;
	static const long REVISION  = 35129;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 17917;
	#define RC_FILEVERSION 6,9,6470,35129
	#define RC_FILEVERSION_STRING "6, 9, 6470, 35129\0"
	static const char FULLVERSION_STRING [] = "6.9.6470.35129";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 70;
	

}
#endif //VERSION_H
