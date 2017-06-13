function OnClick()
	LoadModule(list:GetText())
end

list = UI:NewList("List1", 0, 0, 550, 600)
UI:NewButton("Button1", 10, 560, 30, 90, "Run", OnClick)
local modules = GetFilesList("", "*.module", false)
for i = 1, #modules do
	list:AddItem(modules[i])
end
