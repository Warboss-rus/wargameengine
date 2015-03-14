# Object #

An object is an instance that has coordinates, model and a bunch of other properties. For example, a soilder miniature, a building, a crater - all of these are objects.
In WargameEngine user can interact with objects with a mouse: move objects with a left button and rotate objects with a right button. However, to do this, the object's
model must have a bounding box and it has to be allowed to select and move. The restriction to do this can be achived by disabling object selection and setting a
[Movement Limiter](MovementLimiter.md).

# Functions #

  * **[New](LuaObjectNew.md)** creates a new object.
  * **[NewDecal](LuaObjectNewDecal.md)** creates a new decal.
  * **[GetSelected](LuaObjectGetSelected.md)** returns an object that is currently selected. This function is supposed to be called from the class itself, not its instance.
  * **[GetCount](LuaObjectGetCount.md)** returns a number of objects that currently exists. This function is supposed to be called from the class itself, not its instance.
  * **[GetAt](LuaObjectGetAt.md)** returns an object by index. This function is supposed to be called from the class itself, not its instance.
  * **[Delete](LuaObjectDelete.md)** deletes an object.
  * **[Null](LuaObjectNull.md)** returns if the object is NULL (does not exists).
  * **[GetModel](LuaObjectGetModel.md)** returns object's model.
  * **[GetX](LuaObjectGetX.md)** returns object's x coordinate.
  * **[GetY](LuaObjectGetY.md)** returns object's y coordinate.
  * **[GetZ](LuaObjectGetZ.md)** returns object's z coordinate.
  * **[GetRotation](LuaObjectGetRotation.md)** returns object's rotation.
  * **[Move](LuaObjectMove.md)** moves an object by specified delta.
  * **[Rotate](LuaObjectRotate.md)** rotates an object by specified delta.
  * **[ShowMesh](LuaObjectShowMesh.md)** shows a previously hidden mesh. If a mesh wasn't hidden or does not exists nothing happens.
  * **[HideMesh](LuaObjectHideMesh.md)** hides a mesh so it won't be visible. If a mesh doesn't exists nothing happens.
  * **[GetProperty](LuaObjectGetProperty.md)** retrieves an objects [Property](Property.md). If an object has no such property return an empty string.
  * **[SetProperty](LuaObjectSetProperty.md)** sets an objects [Property](Property.md). If an object already has such property it will be overwritten with new value.
  * **[SetSelectable](LuaObjectSetSelectable.md)** sets if an object can be selected by a mouse.
  * **[SetMoveLimit](LuaObjectSetMoveLimit.md)** sets a [Movement Limiter](MovementLimiter.md).
  * **[SelectNull](LuaObjectSelectNull.md)** deselects currently selected object so calling **GetSelected** will return a NULL object.
  * **[Equals](LuaObjectEquals.md)** checks if two objects are equal.
  * **[IsGroup](LuaObjectIsGroup.md)** checks if an object is actually an [Object Group](ObjectGroup.md).
  * **[GetGroupChildrenCount](LuaObjectGetGroupChildrenCount.md)** returns a number of objects that this group contains. If an object is not a group returns 1. If an object is NULL returns 0.
  * **[GetGroupChildrenAt](LuaObjectGetGroupChildrenAt.md)** returns a child object of the group by its index. If an object is not a group returns NULL object.
