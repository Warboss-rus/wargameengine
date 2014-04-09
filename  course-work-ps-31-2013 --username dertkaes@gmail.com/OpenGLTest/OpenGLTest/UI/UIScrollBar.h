class CUIScrollBar
{
public:
	void Update(int size, int contentSize, int width);
	void Draw() const;
	bool LeftMouseButtonDown(int x, int y);
	bool LeftMouseButtonUp(int x, int y);
	bool IsOnElement(int x, int y) const;
private:
	unsigned int m_position;
	int m_size;
	int m_contentSize;
	int m_width;
	int m_mousePos;
};