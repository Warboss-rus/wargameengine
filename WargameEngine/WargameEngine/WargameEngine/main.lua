function OnClick()
	LoadModule(list:GetText())
	--SetTimedCallback("Callback", 10, false)
end

function Callback()
	
end

local ui = UI:Get()
list = ui:NewList("List1", 0, 0, 550, 600)
local modules = GetFilesList("", "*.module", false)
for i = 1, #modules do
	list:AddItem(modules[i])
end
ui:NewButton("Button1", 10, 560, 30, 90, "Run", "OnClick")