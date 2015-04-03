#include <string>

class IImageDecoder
{
public:
	enum eFlip
	{
		TOP,
		BOTTOM
	};
	enum eColorOrder
	{
		RGBA,
		BGRA
	};
	unsigned int GetWidth() const { return m_width; }
	unsigned int GetHeight() const { return m_height; }
	unsigned int GetBpp() const { return m_bpp; }
	const unsigned char * GetData() const { return m_data; }
	unsigned int GetDataSize() const { return m_dataSize; }
	eFlip GetFlip() const { return m_flip; }
	eColorOrder GetColorOrder() const { return m_colorOrder; }
	void SetID(unsigned int id) { m_id = id; }
	unsigned int GetID() const { return m_id; }
	virtual void Load(std::string const& image) = 0;
protected:
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_bpp;
	unsigned char* m_data;
	unsigned int m_dataSize;
	eFlip m_flip;
	eColorOrder m_colorOrder;
	unsigned int m_id;
};