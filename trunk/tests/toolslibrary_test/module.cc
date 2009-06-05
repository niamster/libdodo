/**
 * vim indentation settings
 * set tabstop=4
 * set shiftwidth=4
 */


#include <libdodo/dodo.h>
#include <iostream>

#ifdef DL_EXT

extern "C"
{
	void
	print(const std::string &str)
	{
		std::cout << str << std::endl;
	}
};

#endif
