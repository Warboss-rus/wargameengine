
```
int LoS(Object source, Object target)
```

## Summary ##
Calculates a visibility percent of one [Object](LuaObjectClass.md) to another. The visibility is calculated by sending 1000 rays from the center of source bounding box to the target's one.
If a ray doesn't intersect other objects it increases visibility. If a target's bounding box is a compound of multiple boxes or the target is an [Object Group](ObjectGroup.md) then the final
visibility is an average of all bounding boxes it contains, no matter what size they are. If the target or the source is a NULL object, the function returns -1.

## Parameters ##
  * **source** - the object that must see the target.
  * **target** - the object whose visiblilty is calculated.

## Output values ##
  * **visibility** - the percent of an target that is visible to a source. Usually is between 0(not visible at all) and 100 (fully visible), but it may return -1 if a source or target is NULL.

## Errors ##
  * **2 argument expected (source, target)**. Parameters number or type does not match.

## Example ##

```
local ob1 = Object:New("model1.obj", 15, 0, 0)
local ob2 = Object:New("model2.obj", -15, 0, 0)
LoS(ob1, ob2) -- returns 100
local ob3 = Object:New("model3.obj", 0, 0, 0)
LoS(ob1, ob2) -- returns less then 100 because ob3 is blocking the visibility
```
Note that LoS(ob1, ob2) may be not equal to LoS(ob2, ob1) because they may have a different bounding boxes.