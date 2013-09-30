#pragma once

#include <string>

class C3DModelUtils
{
public:
	struct FaceIndex
	{
		unsigned int vertex;
		unsigned int normal;
		unsigned int textureCoord;
	};
	static FaceIndex ParseFaceIndex(std::string const& str);
	C3DModelUtils(void);
	~C3DModelUtils(void);
};

