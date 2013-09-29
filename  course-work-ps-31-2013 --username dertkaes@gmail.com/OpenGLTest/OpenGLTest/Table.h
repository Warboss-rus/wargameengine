class CTable
{
public:
	CTable(double height, double width):m_height(height), m_width(width) {}
	void Draw();
private:
	double m_height;
	double m_width;
	unsigned int m_textureID;
};