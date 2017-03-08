//======================================================================================
// Filename: Common.pp
// Description:
//======================================================================================

//======================================================================================
// Includes
//======================================================================================
#include "Common.h"
#include <sstream>
#include <stdexcept>

void ThrowRuntimeError(const std::string &msg, const char* file, int line)
{
	std::string fname(file);
	size_t pos = fname.find_last_of('\\');
	fname = fname.substr(pos + 1, std::string::npos);

	std::ostringstream ostr;
	ostr << "[" << fname << ":" << line << "] " << msg;
	throw std::runtime_error(ostr.str());
}





