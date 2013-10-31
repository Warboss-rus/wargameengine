#include<string>

class CTable
{
public:
	CTable(float width, float height,std::string const& texture):m_height(height / 2), m_width(width / 2), m_texture(texture) {}
	void Draw() const;
	float GetWidth() const { return m_width * 2; }
	float GetHeight() const { return m_height * 2; }
    bool isCoordsOnTable(int worldX, int worldY) const;
private:
	float m_height;
	float m_width;
	std::string m_texture;
	unsigned int m_textureID;
};