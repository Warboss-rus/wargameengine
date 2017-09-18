#pragma once

constexpr char CLASS_VIEWPORT[] = "Viewport";

/*METHODS*/

//Object Viewport:New(int x, int y, int width, int height, float fieldOfView, [bool resize])
//Creates a new onscreen viewport at specified location
constexpr char NEW_VIEWPORT[] = "New";

//Object Viewport:NewOffscreen(int width, int height, float fieldOfView, int textureSlot)
//Creates a new offscreen viewport that renders to the texture. The texture is than available at the given index
constexpr char NEW_OFFSCREEN_VIEWPORT[] = "NewOffscreen";

//int Viewport:Count()
//Returns the number of active viewports
constexpr char VIEWPORT_COUNT[] = "Count";

//Object Viewport:Get(int index)
//returns a viewport by its index
constexpr char GET_VIEWPORT[] = "Get";

//void Viewport:Remove()
//Removes viewport
constexpr char REMOVE_VIEWPORT[] = "Remove";

//Object Viewport:CreateShadowMapViewport(int size, float fovAngle, float x, float y, float z)
constexpr char CREATE_SHADOW_MAP_VIEWPORT[] = "CreateShadowMapViewport";

//void Viewport:DisableShadowMap()
constexpr char DISABLE_SHADOW_MAP[] = "DisableShadowMap";

//void Viewport:SetShadowMapViewport(Object viewport)
constexpr char SET_SHADOW_MAP_VIEWPORT[] = "SetShadowMapViewport";

//void Viewport:EnableFrustumCulling(bool enable)
constexpr char ENABLE_FRUSTUM_CULLING[] = "EnableFrustumCulling";

//void Viewport:CameraStrategy(double maxTransX, double maxTransY, double maxScale, double minScale)
//Creates a third person camera typically seen in strategy games. It can be zoomed with a mouse wheel and moved around via arrow keys. Holding Alt + moving the mouse
//will allow you to turn the camera around. The function arguments determine the translation limits for the camera.
constexpr char CAMERA_STRATEGY[] = "CameraStrategy";

//void Viewport:CameraFirstPerson()
//Creates a first person camera. It can be moved around with arrow keys and zoomed in and out with mouse wheel. Mouse movement is rotating the camera
//Note that the mouse is locked so you won't be able to click buttons, select objects, etc.
constexpr char CAMERA_FIRST_PERSON[] = "CameraFirstPerson";

//void Viewport:SetCameraPosition(float x, float y, float z)
//Sets the position of the camera
constexpr char SET_CAMERA_POSITION[] = "SetCameraPosition";

//void Viewport:SetCameraTarget(float x, float y, float z)
//Sets the target of the camera
constexpr char SET_CAMERA_TARGET[] = "SetCameraTarget";

//void Viewport:SetCameraUpVector(float x, float y, float z)
//Sets the rotation of the camera by specifying up vector
constexpr char SET_CAMERA_UP_VECTOR[] = "SetCameraUpVector";

//void Viewport:CameraAttachToObject(Object object, float offsetX, float offsetY, float offsetZ)
//Attaches camera to object so it will follow object's position and rotation
constexpr char CAMERA_ATTACH_TO_OBJECT[] = "CameraAttachToObject";

//void Viewport:CameraResetInput()
//Removes all input methods from the camera
constexpr char CAMERA_RESET_INPUT[] = "CameraResetInput";

//void Viewport:CameraAttachKeyboardMouse
//Attaches the camera input to keyboard and mouse
constexpr char CAMERA_ATTACH_KEYBOARD_MOUSE[] = "CameraAttachKeyboardMouse";

//Object Viewport:GetCamera()
//returns the camera object
constexpr char GET_CAMERA[] = "GetCamera";


constexpr char CLASS_CAMERA[] = "Camera";

constexpr char ENABLE_TOUCH_MODE[] = "EnableTouchMode";