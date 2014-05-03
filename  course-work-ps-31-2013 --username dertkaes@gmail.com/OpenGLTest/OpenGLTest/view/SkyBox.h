#include <string>

class CSkyBox
{
public:
	CSkyBox(double width, double height, double length, std::string const& imageFolder);
	~CSkyBox();
	void Draw(double x, double y, double z, double scale);
private:
	double m_height;
	double m_width;
	double m_length;
	unsigned int m_list;
};