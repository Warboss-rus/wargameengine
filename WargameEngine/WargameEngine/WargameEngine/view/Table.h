#include<string>

class CTable
{
public:
	CTable(float width, float height, std::string const& texture);
	~CTable();
	void Draw() const;
	const float GetWidth() const { return m_width * 2; }
	const float GetHeight() const { return m_height * 2; }
	std::string const& GetTexture() const { return m_texture; }
    bool isCoordsOnTable(double worldX, double worldY) const;
	void ResetList();
private:
	float m_height;
	float m_width;
	unsigned int m_list;
	std::string m_texture;
};