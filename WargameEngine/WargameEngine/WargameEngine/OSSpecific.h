#include "Typedefs.h"
#include <vector>

namespace wargameEngine
{
//TODO: replace with filesystem based function, move to Utils
std::vector<Path> GetFiles(const Path& path, const Path& mask, bool recursive);
}