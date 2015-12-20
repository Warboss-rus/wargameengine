#pragma once

#define CLASS_OBJECT "Object"

/*METHODS*/

//Object Object:New(string model, double x, double y, double rotation)
//Creates a new object
#define NEW_OBJECT "New"

//Object Object:GetSelected()
//Returns an object currently selected by the user
#define GET_SELECTED_OBJECT "GetSelected"

//long Object:GetCount()
//Returns a total number of object in game model
#define GET_COUNT "GetCount"

//Object Object:GetAt(long index)
//Returns an object by the given index. If no such object exists returns nil
#define GET_AT "GetAt"

//void object:DeleteObject()
//Removes an object from the model. Throws an error if called without an instance.
#define DELETE_OBJECT "Delete"

//std::string object:GetObjectModel()
//Returns the path to object's 3D model. Throws an error if called without an instance.
#define GET_OBJECT_MODEL "GetModel"

//double object:GetX()
//Returns x coordinate of the object. Throws an error if called without an instance.
#define GET_OBJECT_X "GetX"

//double object:GetY()
//Returns y coordinate of the object. Throws an error if called without an instance.
#define GET_OBJECT_Y "GetY"

//double object:GetZ()
//Returns z coordinate of the object. Throws an error if called without an instance.
#define GET_OBJECT_Z "GetZ"

//double object:GetRotation()
//Returns z coordinate of the object. Throws an error if called without an instance.
#define GET_OBJECT_ROTATION "GetRotation"

//void object:Move(double deltaX, double deltaY, double deltaZ)
//Moves the object by a specified delta. Throws an error if called without an instance.
#define MOVE_OBJECT "Move"

//void object:Rotate(double deltaRot)
//Rotates the object by specified delta. Throws an error if called without an instance.
#define ROTATE_OBJECT "Rotate"

//void object:ShowMesh(string mesh)
//Enables visibility of specified part of 3d model. Throws an error if called without an instance.
#define SHOW_MESH "ShowMesh"

//void object:HideMesh(string mesh)
//Disables visibility of specified part of 3d model. Throws an error if called without an instance.
#define HIDE_MESH "HideMesh"

//string object:GetProperty(string key)
//Returns a user property of the object by a given key. Throws an error if called without an instance.
#define GET_PROPERTY "GetProperty"

//string object:SetProperty(string key, string value)
//Sets a user property of the object by a given key. Throws an error if called without an instance.
#define SET_PROPERTY "SetProperty"

//void object:SetSelectable(bool selectable)
//Determines if object is selectable by left-clicking on it. Throws an error if called without an instance.
#define SET_SELECTABLE "SetSelectable"

//void SetMoveLimit(string moveLimiterType, [limiter specific args])
//Limits the movement of an object by specified movement limiter. Throws an error if called without an instance. Possible moveLimiterType values:
//"free" - No limits. No additional arguments.
//"static" - An object will be stuck at its current position. No additional arguments.
//"circle" - Limits object movement inside a specified circle. Arguments: double centerX, double centerY, double radius
//"rectangle" - Limits object movement inside a specified rectangle. Arguments: double x1, double y1, double x2, double y2
//"tiles" - Limits object position to integers. Doesn't limit rotation. No additional arguments.
//"custom" - Allows you to use script function to limit object position and rotation. This function should return 4 values in an array: 3 position components and a rotation
//Arguments: array<double> function(array<double> position, double rotation, array<double> prevPosition, double prevRotation)
#define SET_MOVE_LIMIT "SetMoveLimit"

//void Select(Object select)
//Make object selected. Pass nil to deselect all objects. Doesn't require an instance.
#define SELECT_OBJECT "Select"

//bool ObjectEquals(Object otherObject)
//Compare one Object instance to another.
#define OBJECT_EQUALS "ObjectEquals"

//bool IsGroup()
//Checks if Object instance is an object group. Throws an error if called without an instance.
#define OBJECT_IS_GROUP "IsGroup"

//long GetGroupChildrenCount()
//Returns a number of children an Object group has. If its not an Object group, returns 1. If the instance is nil, returns 0
#define GET_GROUP_CHILDREN_COUNT "GetGroupChildrenCount"

//Object GetGroupChildrenAt(long index)
//Returns a child Object of an Object group. If its not an Object group, returns nil. Throws an error if called without an instance. 
#define GET_GROUP_CHILDREN_AT "GetGroupChildrenAt"

//void PlayAnimation(string animation, string loopMode, float speed)
//Object plays the animation with specified parameters. loopMode can be: "nonlooping", "looping" or "holdend". Throws an error if called without an instance. 
#define PLAY_ANIMATION "PlayAnimation"

//array<string> GetAnimations()
//Returns a list of animation that model associated with this object has. Throws an error if called without an instance. 
#define GET_ANIMATIONS "GetAnimations"

//void AdditionalModel(string model)
//Adds an additional model to an object (for example a weapon). An object can have one main model and unlimited number of additional models. 
//Note that this additional model has no bounding box, so it doesn't affects selection, collision, and LoS determination. Throws an error if called without an instance. 
#define ADDITIONAL_MODEL "AdditionalModel"

//void RemoveAdditionalModel(string model)
//Removes an additional model from an object. If object had no such model, does nothing. Throws an error if called without an instance. 
#define REMOVE_ADDITIONAL_MODEL "RemoveAdditionalModel"

//void GoTo(double x, double y, double speed, string animation, float animationSpeed)
//Makes the object smoothly move to a specified location while playing the specified animation. Throws an error if called without an instance. 
#define GO_TO "GoTo"

//void ApplyTeamColor(string suffix, unsigned char r, unsigned char g, unsigned char b)
//Applies a teamcolor to object's textures. The suffix provided determines the filename of the mask file, so for texture "texture.dds" with a suffix of "_mask1" the mask filename
//will be "texture_mask1.bmp". This mask when used to blend original texture with the specified color). Throws an error if called without an instance. 
#define APPLY_TEAMCOLOR "ApplyTeamColor"

//void ReplaceTexture(string oldTextre, string newTexture)
//Replaces one texture that object has with another one.
#define REPLACE_TEXTURE "ReplaceTexture"

/*PROPERTIES*/

//double x
//Gets or sets x component of object's coordinates. Throws an error if called without an instance. 
#define PROPERTY_X "x"

//double y
//Gets or sets y component of object's coordinates. Throws an error if called without an instance. 
#define PROPERTY_Y "y"

//double z
//Gets or sets x component of object's coordinates. Throws an error if called without an instance. 
#define PROPERTY_Z "z"

//double rotation
//Gets or sets object's rotation. Throws an error if called without an instance. 
#define PROPERTY_ROTATION "rotation"

//bool selectable
//Gets or sets whenether object is selectable by the user. Throws an error if called without an instance. 
#define PROPERTY_SELECTABLE "selectable"

//long count
//Returns a total number of object in game model
#define PROPERTY_COUNT "count"