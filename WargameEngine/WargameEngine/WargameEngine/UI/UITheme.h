#pragma once
#include "../Typedefs.h"

struct CUITheme
{
	CUITheme();
	void Load(const Path& filename);

	Path texture;
	float defaultColor[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
	float textfieldColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	struct Text
	{
		float color[4] = {0.0f, 0.0f, 0.0f, 1.0f};
		std::string font = "times.ttf";
		unsigned int fontSize = 18;
		enum class Aligment
		{
			left = 0,
			center,
			right
		} aligment = Aligment::left;
	} text;
	struct Button
	{
	public:
		float texCoord[4] = { 0.02344f, 0.59375f, 0.22266f, 0.53516f };
		float pressedTexCoord[4] = { 0.02344f, 0.66016f, 0.22266f, 0.60157f };
		Text text;
	} button;
	struct ComboBox
	{
		float texCoord[4] = { 0.27734f, 0.80469f, 0.3125f, 0.74219f };
		float expandedTexCoord[4] = { 0.27734f, 0.74219f, 0.3125f, 0.80469f };
		float buttonWidthCoeff = 0.66f;
		int borderSize = 2;
		int elementSize = 25;
		Text text;
	} combobox;
	struct List
	{
		int borderSize = 2;
		int elementSize = 20;
		float selectionColor[4] = { 0.2f, 0.2f, 1.0f, 1.0f };
		Text text;
	} list;
	struct CheckBox
	{
		float texCoord[4] = { 0.121f, 0.789f, 0.168f, 0.7422f };
		float checkedTexCoord[4] = { 0.121f, 0.844f, 0.168f, 0.797f };
		float checkboxSizeCoeff = 1.0f;
		Text text;
	} checkbox;
	struct Edit
	{
		int borderSize = 2;
		float selectionColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
		Text text;
	} edit;
	struct RadioGroup
	{
		float texCoord[4] = { 0.379f, 0.793f, 0.429f, 0.742f };
		float selectedTexCoord[4] = { 0.379f, 0.8515f, 0.429f, 0.8f};
		float buttonSize = 15.0f;
		float elementSize = 22.0f;
		Text text;
	} radiogroup ;
	struct ScrollBar
	{
		float texCoord[4] = { 0.07f, 0.984f, 0.117f, 0.953f };
		float pressedTexCoord[4] = { 0.121f, 0.984f, 0.168f, 0.953f };
		int width = 20;
		int buttonSize = 20;
		float backgroundColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
		float barColor[4] = { 0.75f, 0.75f, 0.75f };
	} sbar;
	struct Window
	{
		int headerHeight = 20;
		int buttonSize = 20;
		float closeButtonTexCoord[4] = { 0.6758f, 0.996f, 0.7343f, 0.9453f };
		float headerColor[4] = { 0.4f, 0.4f, 1.0f, 1.0f };
		Text headerText;
	} window;
};