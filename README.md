# TUNG Router
A PCB routing program for The Ultimate Nerd Game. Still very WIP.

# Usage
```
router.exe [-b] filename
```
When the `-b` flag is set, the router tries to find the best routing
possible. This is a very intensive process and currently it doesn't
have a terminating condition. Its only use is for testing optimization
techniques at the moment.

:children_crossing: The if the `-b` flag is given, the router will not generate a board!

# Table of contents

  * [Circuit Description Files](#circuit-description-files)
    * [Examples](#examples)
  * [Building From Source](#building-from-source)
    * [Building Tests](#building-tests)

# Circuit Description Files

:children_crossing: The syntax may change in the close future.

All CDF must start with `board width height`

### Defining a sub-board
`chip name width height` creates a new sub-board

`pin chip_name side offset name` adds a pin to that sub-board

* `side` can either be `n`, `s`, `e`, or `w`

* `offset` is the offset from the top-left corner.

### Placing a sub-board
`instance chip_name x y name` places a board with `x` and `y` indicating the
top-left corner of the board.

### Connecting pins
`net name instance_name pin_name` adds pin `pin_name` of sub-board `instance_name` to
the network called `name`.

All pins on the same network will be connected together.

### Useful tip
If you want to add connections on the side of the board, you can define a chip
of width or height 0 with pins on only one side, and place it at the edge of the board.

## Examples

```
TODO, sorry
```

# Building From Source

This project requires `gcc` with support for C++17, and `make`

Dependencies:
  * [SDL2](https://www.libsdl.org/download-2.0.php)
  * That's it

Create a `lib` folder at the root of the project and place the `include` and `lib`
folders of SDL2 inside.

Then just do `make`.

## Building Tests

This project uses the [Catch library](https://github.com/catchorg/Catch2)
for unit testing.
Place `catch.hpp` in `lib/include` and do `make test`.
