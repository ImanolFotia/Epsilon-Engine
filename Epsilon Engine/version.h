#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "12";
	static const char MONTH[] = "07";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.07";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 10;
	static const long MINOR  = 8;
	static const long BUILD  = 10785;
	static const long REVISION  = 58905;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 31442;
	#define RC_FILEVERSION 10,8,10785,58905
	#define RC_FILEVERSION_STRING "10, 8, 10785, 58905\0"
	static const char FULLVERSION_STRING [] = "10.8.10785.58905";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 85;
	

}
#endif //VERSION_H
