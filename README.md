GCodeWorkShop
============

This is a fork of [EdytorNC](https://github.com/artur3/EdytorNC), a text editor
for CNC programmers.

- [Some features](#some-features)
- [Some shortcuts](#some-shortcuts)
- [Getting GCodeWorkShop](#getting-gcodeworkshop)
- [Command line options](#command-line-options)
- [License](#license)


Some features
------------

* Code colourizing.
* Sessions - to save/restore open file list
* Send/receive through serial port with automatic file saving and Serial port
  file server - possibility to download new CNC program directly from machine.
* Find in files
* Find/Replace can be used to addition, subtraction, multiplication, division
  on numbers or highlighting text.  
  in Find: `Y$$` in Replace `$$*2` will multiply by 2 all numbers found with `Y`  
  in Find: `Y$100$10` in Replace `$$*2` will multiply by 2 only numbers between
  `10` and `100` found with `Y`  
  Find can be used also to highlight specific program blocks
* Simple project management - you can keep links to (almost) all files related
  to your part program in one place.
* First comment in program is displayed in title bar. In first comment you can
  write part name.
* Comment with date will be replaced with current date on file save. Comment
  format: (dd-/.mm-/.yyyy) or ;dd-/.mm-/.yyyy - now system localization settings are used.
* Block renumbering - whole program or selection
* Conversion inch/mm, mm/inch. You can write inches in format `1.3/4` `1/4` ...
* Speed, feed calculation
* Bolt holes calculation
* Solutions of triangles calculation


Some shortcuts
------------

* `Ctrl+;` Comments/uncomments selected text with semicolon ;
* `Ctrl+9` Comments/uncomments selected text with parentheses ()
* `Ctrl+2` Adds Block Skip (/) and/or increments it
* `Ctrl+3` Adds Block Skip (/) and/or decrements it
* `Ctrl+1` Removes Block Skip (/)
* `Ctrl+0` or `Ctrl+Double Click` In line calculator, you can do some math
  operations directly in editor. You can select address (eg `X123.45`) before
  pressing `Ctrl+0` (`Ctrl+Double Click` will do the same), value will be
  entered in the calc.  
  After pressing `Enter` (and if no error) result will be pasted in cursor
  position. Supported operators: `+` `-` `*` `/` `SIN(x)` `COS(x)` `TAN(x)`
  `SQRT(x)` `SQR(x)` `ABS(x)` `TRUNC(x)` `PI`


Getting GCodeWorkShop
---------------------

On the [release page](https://github.com/GCodeProjects/GCodeWorkShop/releases/latest)
you can find binary builds for Debian, Ubuntu and Windows. For building the
application yourself, the [build instructions](INSTALL.md) may be useful.

In addition, third-party builds are available:

- For [ALT Linux](https://packages.altlinux.org/ru/sisyphus/srpms/gcodeworkshop/),
  thanks to Anton Midyukov (antohami).
- For [OpenSuse](https://build.opensuse.org/package/show/home:AndnoVember:LXQt:Qt6/gcodeworkshop),
  thanks to Андрей Алыпов (AndnoVember).

[![Packaging status](https://repology.org/badge/vertical-allrepos/gcodeworkshop.svg)](https://repology.org/project/gcodeworkshop/versions)


Command line options
------------------

- open file -> `gcodeworkshop file.nc`
- diff two files -> `gcodeworkshop -diff file1.nc file2.nc`


## License

GCodeWorkShop is distributed under the GNU LGPL version 3 license. You can get
the source code at the following sites:

- [GitHub](https://github.com/GCodeProjects/GCodeWorkShop)
- [GitFlic](https://gitflic.ru/project/u-235/gcodeworkshop)
- [Codeberg](https://codeberg.org/u-235/GCodeWorkShop)
