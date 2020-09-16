# bf86
[![](https://github.com/lemon-sherbet/bf86/workflows/CI/badge.svg)](https://github.com/lemon-sherbet/bf86/actions)

A brainfuck -> x86 compiler (linux only).

Build with `make`.

# Usage

```sh
./bf86 "+[.+]" > a.out
chmod +x a.out
./a.out
# or 
./bf86 < test/hello.bf > a.out
chmod +x a.out
./a.out
```

Cells are 8-bit, with wrapping behaviour. 30720 cells are allocated. Moving the pointer outside of the bounds of the array will (most likely) cause a segmentation fault.

Behaviour of `,` on EOF is to leave the current cell unchanged.
