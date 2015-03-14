# 1. Create your .module file #

Create an text file with an extension .module that must be stored in the same folder as WargameEngine.exe. It will contain information where all content for this module is stored. It must contain several key-value pairs divided with ' = '. Currently availible keys:
  * **Name** - name of your module.
  * **Version** - version of the module. _integer!_
  * **Author** - author of the module.
  * **Site** - site link.
  * **Playable** - Mod is visible. _Must be 0 or 1_
  * **Folder** - Specifies the main folder of the content. May be absoule or relative path. If empty the folder is set to the .exe location.
  * **Script** - Specifies the script that runs at module startup. The path is relative from **Folder**. If empty than main.lua runs.
  * **Models** - Specifies the folder where all models are stored. The path is relative from **Folder**. If empty than models folder is equal to main folder.
  * **Textures** - Specifies the folder where all textures are stored. The path is relative from **Folder**. If empty than textures folder is equal to the main folder.
  * **Shaders** - Specifies the folder where all shaders are stored. The path is relative from **Folder**. If empty than shaders folder is equal to the main folder.
To start a line comment preceed it with ';'.
Example:
```
Name = Test
Version = 1
Author = Warboss-rus
Site = https://code.google.com/p/wargameengine ; comment here
Playable = 1
Folder = Test
Script = test.lua
Models = models
Textures = texture
Shaders = 
```

# 2. Create content #

All content must be stored in folder specified with **Folder** key in the module (see step 1). The module can contain following types of content:
  * [.obj and .wbm Models](Models.md)
  * [Textures](Textures.md)
  * [LUA Scripts](LUA.md)
  * [GLSL Shaders](Shaders.md)
For more information about the content see individual pages for all types of content.

# 3. Run you .module #

To run your module you must run WargameEngine.exe with a _-module_ parameter. For example:
`WargameEngine.exe -module test.module`