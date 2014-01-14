#include <string>

class CSkyBox
{
public:
	CSkyBox(float width, float height, float length, std::string const& imageFolder);
	void Draw(float x, float y, float z, float scale);
private:
	float m_height;
	float m_width;
	float m_length;
	std::string m_imageFolder;
};