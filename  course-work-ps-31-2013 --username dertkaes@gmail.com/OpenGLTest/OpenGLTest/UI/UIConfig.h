#include <string>

class CUIConfig
{
public:
	static void SetDefault();
	static std::string texture;
	static void * font;
	static int fontHeight;
	static float defaultColor[3];
	static float textfieldColor[3];
	static float textColor[3];
	struct sButton
	{
	public:
		double texCoord[4];
		double pressedTexCoord[4];
		sButton();
	};
	static sButton button;
	struct sListBox
	{
		double texCoord[4];
		double expandedTexCoord[4];
		double buttonWidthCoeff;
		int borderSize;
		sListBox();
	};
	static sListBox listbox;
	struct sCheckBox
	{
		double texCoord[4];
		double checkedTexCoord[4];
		double checkboxSizeCoeff;
		sCheckBox();
	};
	static sCheckBox checkbox;
};