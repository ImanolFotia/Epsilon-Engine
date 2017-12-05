#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "21";
	static const char MONTH[] = "10";
	static const char YEAR[] = "2017";
	static const char UBUNTU_VERSION_STYLE[] =  "17.10";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 9;
	static const long MINOR  = 0;
	static const long BUILD  = 8884;
	static const long REVISION  = 48415;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 25610;
	#define RC_FILEVERSION 9,0,8884,48415
	#define RC_FILEVERSION_STRING "9, 0, 8884, 48415\0"
	static const char FULLVERSION_STRING [] = "9.0.8884.48415";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 84;
	

}
#endif //VERSION_H
