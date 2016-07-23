#pragma once

#define CLASS_OBJECT L"Object"

/*METHODS*/

//Object Object:New(string model, double x, double y, double rotation)
//Creates a new object
#define NEW_OBJECT L"New"

//Object Object:GetSelected()
//Returns an object currently selected by the user
#define GET_SELECTED_OBJECT L"GetSelected"

//long Object:GetCount()
//Returns a total number of object in game model
#define GET_COUNT L"GetCount"

//Object Object:GetAt(long index)
//Returns an object by the given index. If no such object exists returns nil
#define GET_AT L"GetAt"

//void object:DeleteObject()
//Removes an object from the model. Throws an error if called without an instance.
#define DELETE_OBJECT L"Delete"

//std::string object:GetObjectModel()
//Returns the path to object's 3D model. Throws an error if called without an instance.
#define GET_OBJECT_MODEL L"GetModel"

//double object:GetX()
//Returns x coordinate of the object. Throws an error if called without an instance.
#define GET_OBJECT_X L"GetX"

//double object:GetY()
//Returns y coordinate of the object. Throws an error if called without an instance.
#define GET_OBJECT_Y L"GetY"

//double object:GetZ()
//Returns z coordinate of the object. Throws an error if called without an instance.
#define GET_OBJECT_Z L"GetZ"

//double object:GetRotation()
//Returns z coordinate of the object. Throws an error if called without an instance.
#define GET_OBJECT_ROTATION L"GetRotation"

//void object:Move(double deltaX, double deltaY, double deltaZ)
//Moves the object by a specified delta. Throws an error if called without an instance.
#define MOVE_OBJECT L"Move"

//void object:Rotate(double deltaRot)
//Rotates the object by specified delta. Throws an error if called without an instance.
#define ROTATE_OBJECT L"Rotate"

//void object:ShowMesh(string mesh)
//Enables visibility of specified part of 3d model. Throws an error if called without an instance.
#define SHOW_MESH L"ShowMesh"

//void object:HideMesh(string mesh)
//Disables visibility of specified part of 3d model. Throws an error if called without an instance.
#define HIDE_MESH L"HideMesh"

//string object:GetProperty(string key)
//Returns a user property of the object by a given key. Throws an error if called without an instance.
#define GET_PROPERTY L"GetProperty"

//string object:SetProperty(string key, string value)
//Sets a user property of the object by a given key. Throws an error if called without an instance.
#define SET_PROPERTY L"SetProperty"

//void object:SetSelectable(bool selectable)
//Determines if object is selectable by left-clicking on it. Throws an error if called without an instance.
#define SET_SELECTABLE L"SetSelectable"

//void SetMoveLimit(string moveLimiterType, [limiter specific args])
//Limits the movement of an object by specified movement limiter. Throws an error if called without an instance. Possible moveLimiterType values:
//"free" - No limits. No additional arguments.
//"static" - An object will be stuck at its current position. No additional arguments.
//"circle" - Limits object movement inside a specified circle. Arguments: double centerX, double centerY, double radius
//"rectangle" - Limits object movement inside a specified rectangle. Arguments: double x1, double y1, double x2, double y2
//"tiles" - Limits object position to integers. Doesn't limit rotation. No additional arguments.
//"custom" - Allows you to use script function to limit object position and rotation. This function should return 4 values in an array: 3 position components and a rotation
//Arguments: array<double> function(array<double> position, double rotation, array<double> prevPosition, double prevRotation)
#define SET_MOVE_LIMIT L"SetMoveLimit"

//void Select(Object select)
//Make object selected. Pass nil to deselect all objects. Doesn't require an instance.
#define SELECT_OBJECT L"Select"

//bool ObjectEquals(Object otherObject)
//Compare one Object instance to another.
#define OBJECT_EQUALS L"ObjectEquals"

//bool IsGroup()
//Checks if Object instance is an object group. Throws an error if called without an instance.
#define OBJECT_IS_GROUP L"IsGroup"

//long GetGroupChildrenCount()
//Returns a number of children an Object group has. If its not an Object group, returns 1. If the instance is nil, returns 0
#define GET_GROUP_CHILDREN_COUNT L"GetGroupChildrenCount"

//Object GetGroupChildrenAt(long index)
//Returns a child Object of an Object group. If its not an Object group, returns nil. Throws an error if called without an instance. 
#define GET_GROUP_CHILDREN_AT L"GetGroupChildrenAt"

//void PlayAnimation(string animation, string loopMode, float speed)
//Object plays the animation with specified parameters. loopMode can be: L"nonlooping", L"looping" or L"holdend". Throws an error if called without an instance. 
#define PLAY_ANIMATION L"PlayAnimation"

//array<string> GetAnimations()
//Returns a list of animation that model associated with this object has. Throws an error if called without an instance. 
#define GET_ANIMATIONS L"GetAnimations"

//void AdditionalModel(string model)
//Adds an additional model to an object (for example a weapon). An object can have one main model and unlimited number of additional models. 
//Note that this additional model has no bounding box, so it doesn't affects selection, collision, and LoS determination. Throws an error if called without an instance. 
#define ADDITIONAL_MODEL L"AdditionalModel"

//void RemoveAdditionalModel(string model)
//Removes an additional model from an object. If object had no such model, does nothing. Throws an error if called without an instance. 
#define REMOVE_ADDITIONAL_MODEL L"RemoveAdditionalModel"

//void GoTo(double x, double y, double speed, string animation, float animationSpeed)
//Makes the object smoothly move to a specified location while playing the specified animation. Throws an error if called without an instance. 
#define GO_TO L"GoTo"

//void ApplyTeamColor(string suffix, unsigned char r, unsigned char g, unsigned char b)
//Applies a teamcolor to object's textures. The suffix provided determines the filename of the mask file, so for texture L"texture.dds" with a suffix of L"_mask1" the mask filename
//will be L"texture_mask1.bmp". This mask when used to blend original texture with the specified color). Throws an error if called without an instance. 
#define APPLY_TEAMCOLOR L"ApplyTeamColor"

//void ReplaceTexture(string oldTextre, string newTexture)
//Replaces one texture that object has with another one.
#define REPLACE_TEXTURE L"ReplaceTexture"

/*PROPERTIES*/

//double x
//Gets or sets x component of object's coordinates. Throws an error if called without an instance. 
#define PROPERTY_X L"x"

//double y
//Gets or sets y component of object's coordinates. Throws an error if called without an instance. 
#define PROPERTY_Y L"y"

//double z
//Gets or sets x component of object's coordinates. Throws an error if called without an instance. 
#define PROPERTY_Z L"z"

//double rotation
//Gets or sets object's rotation. Throws an error if called without an instance. 
#define PROPERTY_ROTATION L"rotation"

//bool selectable
//Gets or sets whenether object is selectable by the user. Throws an error if called without an instance. 
#define PROPERTY_SELECTABLE L"selectable"

//long count
//Returns a total number of object in game model
#define PROPERTY_COUNT L"count"