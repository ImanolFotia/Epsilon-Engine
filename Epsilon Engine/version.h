#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion{
	
	//Date Version Types
	static const char DATE[] = "05";
	static const char MONTH[] = "04";
	static const char YEAR[] = "2018";
	static const char UBUNTU_VERSION_STYLE[] =  "18.04";
	
	//Software Status
	static const char STATUS[] =  "Alpha";
	static const char STATUS_SHORT[] =  "a";
	
	//Standard Version Type
	static const long MAJOR  = 9;
	static const long MINOR  = 10;
	static const long BUILD  = 9860;
	static const long REVISION  = 53807;
	
	//Miscellaneous Version Types
	static const long BUILDS_COUNT  = 29125;
	#define RC_FILEVERSION 9,10,9860,53807
	#define RC_FILEVERSION_STRING "9, 10, 9860, 53807\0"
	static const char FULLVERSION_STRING [] = "9.10.9860.53807";
	
	//These values are to keep track of your versioning state, don't modify them.
	static const long BUILD_HISTORY  = 60;
	

}
#endif //VERSION_H
