#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "21";
	static const char MONTH[] = "06";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.06";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 10;
	static const long MINOR  = 8;
	static const long BUILD  = 10780;
	static const long REVISION  = 58873;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 31428;
	#define RC_FILEVERSION 10,8,10780,58873
	#define RC_FILEVERSION_STRING "10, 8, 10780, 58873\0"
	static const char FULLVERSION_STRING [] = "10.8.10780.58873";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 80;
	

}
#endif //VERSION_H
