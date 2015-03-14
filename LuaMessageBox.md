
```
void MessageBox(string text)
void MessageBox(string text, string caption)
```

## Summary ##
Displays a messagebox with a specified text. If you don't specify a caption if will be empty. Messagebox are not shown when in [fullscreen mode](WargameEngine#FullScreen.md).

## Parameters ##
  * **text** - a text to show inside a messagebox.
  * **caption** - the caption of a messagebox.

## Output values ##
NONE

## Errors ##
  * **1 or 2 argument expected (text, caption)**. Parameters number or type does not match.

## Examples ##
```
MessageBox("Hello world!")
```
Shows a messagebox with an empty caption and a "Hello world!" text.
```
MessageBox("Hello world!", "Foo bar")
```
Shows a messagebox with a "Foo bar" caption and a "Hello world!" text.