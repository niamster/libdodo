#include <libdodo/directives.h>

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
