#include "C3DModelUtils.h"
#include <sstream>

C3DModelUtils::C3DModelUtils(void)
{
}


C3DModelUtils::~C3DModelUtils(void)
{
}

unsigned int ParseStringUntilSlash(std::stringstream& indexStream, char ch = 0)
{
	std::string index;
	if (ch == 0)
	{
		std::getline(indexStream, index);
	}
	else
	{
		std::getline(indexStream, index, ch);
	}
	return (!index.empty()) ? atoi(index.c_str()): 0;
}

C3DModelUtils::FaceIndex C3DModelUtils::ParseFaceIndex(std::string const& str)
{
	std::stringstream indexStream(str);
	std::string index;
	FaceIndex res;
	res.vertex = ParseStringUntilSlash(indexStream, '/');
	res.textureCoord = ParseStringUntilSlash(indexStream, '/');
	res.normal = ParseStringUntilSlash(indexStream);
	return res;
}

