#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "25";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.04";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 10;
	static const long MINOR  = 5;
	static const long BUILD  = 10458;
	static const long REVISION  = 57078;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 30576;
	#define RC_FILEVERSION 10,5,10458,57078
	#define RC_FILEVERSION_STRING "10, 5, 10458, 57078\0"
	static const char FULLVERSION_STRING [] = "10.5.10458.57078";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 58;
	

}
#endif //VERSION_H
