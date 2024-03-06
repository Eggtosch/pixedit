# pixedit

A simple and stupid pixelart editor.
There are palette files and the actual pixelart files, both use text.

## using the application

Select a color from the palette on the right.
Left click on a pixel to set the color to the currently selected one.
Right click on a pixel to delete the color.

## palette files

A palette files consists of multiple lines where each line contains one color of the palette, e.g.:

```
0xff000000 # black
0xffffffff # white
0xffff0000 # red
0xff00ff00 # green
0xff0000ff # blue
```

All colors are in ARGB format.
Everything after a # on a line is ignored.

## pixelart files

Create a pixelart file from a palette file with `pixedit --create <palette-file> <width>x<height> <pixelart-file>`.

The first line of a pixelart contains the palette as hexadecimal values separated by commas.
The second line contains the dimensions of the image, in `<width>x<height>` format.
Each of the following lines denotes a line of the image, with values separated by commas.
Each value is an index into the color palette, where 0 means unpopulated and 1 is the first color of the palette.
An example pixelart file looks like the following:

```
0x00000000,0xff000000,0xffffffff,0xffff0000,0xff00ff00,0xff0000ff
8x8
04,05,05,05,05,05,05,03
05,04,01,01,01,01,03,05
05,01,04,02,02,03,01,05
05,01,02,04,03,02,01,05
05,01,02,03,04,02,01,05
05,01,03,02,02,04,01,05
05,03,01,01,01,01,04,05
03,05,05,05,05,05,05,04
```

## other pixedit commands

Convert a pixelart file to a png with `pixedit --to-png <pixelart-file>`.

Change the palette of a pixelart file with `pixedit --update-palette <palette-file> <pixelart-file>`.
