
```
void CreateTable(float width, float height, string texture)
```

## Summary ##
Creates a table of specified size and texture. The table is always in z=0 pane.

## Parameters ##
  * **width** - the width of the table created.
  * **height** - the height of the table created.
  * **texture** - the name of texture that applies to a table surface.

## Output values ##
NONE

## Errors ##
  * **3 argument expected (width, height, texture)**. Parameters number or type does not match.

## Example ##

```
CreateTable(30, 15, "sand.bmp")
```
Creates a table with a width of 30, the height of 15 and "sand.bmp" as a surface texture.