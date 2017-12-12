#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "10";
	static const char MONTH[] = "12";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.12";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 9;
	static const long MINOR  = 2;
	static const long BUILD  = 9017;
	static const long REVISION  = 49147;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 26143;
	#define RC_FILEVERSION 9,2,9017,49147
	#define RC_FILEVERSION_STRING "9, 2, 9017, 49147\0"
	static const char FULLVERSION_STRING [] = "9.2.9017.49147";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 17;
	

}
#endif //VERSION_H
