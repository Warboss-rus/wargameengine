#include<vector>

class C3DModel
{
public:
	C3DModel(std::string const& path);
	void Draw();
private:
	std::vector<double> m_vertices;
	std::vector<double> m_textureCoords;
	std::vector<double> m_normals;
	std::vector<unsigned int> m_faces;
};