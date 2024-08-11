# C simple REPL implementation

Run the simple REPL using the following command from this directory:

```
gcc src/interpreter.c -o interpreter.exe -Wno-discarded-qualifiers
interpreter.exe
```

Requires gcc C-compiler.

Note: the interpreter only understands ints and not floats and as such only supports integer division.
