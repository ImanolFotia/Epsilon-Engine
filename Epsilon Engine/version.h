#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "21";
	static const char MONTH[] = "08";
	static const char YEAR[] = "2016";
	static const char UBUNTU_VERSION_STYLE[] =  "16.08";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 5;
	static const long MINOR  = 7;
	static const long BUILD  = 5151;
	static const long REVISION  = 27911;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 14117;
	#define RC_FILEVERSION 5,7,5151,27911
	#define RC_FILEVERSION_STRING "5, 7, 5151, 27911\0"
	static const char FULLVERSION_STRING [] = "5.7.5151.27911";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 51;
	

}
#endif //VERSION_H
