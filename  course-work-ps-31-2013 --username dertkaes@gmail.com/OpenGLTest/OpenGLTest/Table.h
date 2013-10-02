#include<string>

class CTable
{
public:
	CTable(float width, float height,std::string const& texture):m_height(height / 2), m_width(width / 2), m_texture(texture) {}
	void Draw();
private:
	float m_height;
	float m_width;
	std::string m_texture;
	unsigned int m_textureID;
};