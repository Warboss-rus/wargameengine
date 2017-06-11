#pragma once

#define CLASS_VIEWPORT L"Viewport"

/*METHODS*/

//Object Viewport:New(int x, int y, int width, int height, float fieldOfView, [bool resize])
//Creates a new onscreen viewport at specified location
#define NEW_VIEWPORT L"New"

//Object Viewport:NewOffscreen(int width, int height, float fieldOfView, int textureSlot)
//Creates a new offscreen viewport that renders to the texture. The texture is than available at the given index
#define NEW_OFFSCREEN_VIEWPORT L"NewOffscreen"

//int Viewport:Count()
//Returns the number of active viewports
#define VIEWPORT_COUNT L"Count"

//Object Viewport:Get(int index)
//returns a viewport by its index
#define GET_VIEWPORT L"Get"

//void Viewport:Remove()
//Removes viewport
#define REMOVE_VIEWPORT L"Remove"

//Object Viewport:CreateShadowMapViewport(int size, float fovAngle, float x, float y, float z)
#define CREATE_SHADOW_MAP_VIEWPORT L"CreateShadowMapViewport"

//void Viewport:DisableShadowMap()
#define DISABLE_SHADOW_MAP L"DisableShadowMap"

//void Viewport:SetShadowMapViewport(Object viewport)
#define SET_SHADOW_MAP_VIEWPORT L"SetShadowMapViewport"

//void Viewport:EnableFrustumCulling(bool enable)
#define ENABLE_FRUSTUM_CULLING L"EnableFrustumCulling"

//void Viewport:CameraStrategy(double maxTransX, double maxTransY, double maxScale, double minScale)
//Creates a third person camera typically seen in strategy games. It can be zoomed with a mouse wheel and moved around via arrow keys. Holding Alt + moving the mouse
//will allow you to turn the camera around. The function arguments determine the translation limits for the camera.
#define CAMERA_STRATEGY L"CameraStrategy"

//void Viewport:CameraFirstPerson()
//Creates a first person camera. It can be moved around with arrow keys and zoomed in and out with mouse wheel. Mouse movement is rotating the camera
//Note that the mouse is locked so you won't be able to click buttons, select objects, etc.
#define CAMERA_FIRST_PERSON L"CameraFirstPerson"

//void Viewport:SetCameraPosition(float x, float y, float z)
//Sets the position of the camera
#define SET_CAMERA_POSITION L"SetCameraPosition"

//void Viewport:SetCameraTarget(float x, float y, float z)
//Sets the target of the camera
#define SET_CAMERA_TARGET L"SetCameraTarget"

//void Viewport:SetCameraUpVector(float x, float y, float z)
//Sets the rotation of the camera by specifying up vector
#define SET_CAMERA_UP_VECTOR L"SetCameraUpVector"

//void Viewport:CameraAttachToObject(Object object, float offsetX, float offsetY, float offsetZ)
//Attaches camera to object so it will follow object's position and rotation
#define CAMERA_ATTACH_TO_OBJECT L"CameraAttachToObject"

//Object Viewport:GetCamera()
//returns the camera object
#define GET_CAMERA L"GetCamera"


#define CLASS_CAMERA L"Camera"

#define ENABLE_TOUCH_MODE L"EnableTouchMode"