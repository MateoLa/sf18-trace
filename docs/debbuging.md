<div align="center">

<img src=/assets/chess-crash.webp width="128"></img>

<h3>Debbuging Stockfish-18 WebAssembly</h3>

<p>Summary of incorrect compilation options and errors due to the nature of C++ for WebAssembly</p>

</div>


#### wasm-ld: error: unable to find library -lgcov

Emscripten uses Clang as its underlying C and C++ compiler. -lgcov is not supported by clang/llvm for code coverage.<br>
Replace -lgcov with --coverage flag.


#### Memory access out of bounds

Compiling with the option `debug=yes` the error points to:
    <p color="red">bitboard.cpp:146:29  --> reference[size] = Bitboards::sliding_attack(pt, s, b);</p>
    bitboard.cpp:79  --> init_magics(ROOK, RookTable, Magics);

Compiling with the option `-fsanitize=undefined,address` the browser reports: `AddressSanitizer: out of memory: allocator is trying to allocate 0xa0100000 bytes` (This is 2685403136 bytes or 2,5 GB).


#### Stockfish Memory access out of bounds Manual Trace

```C++
main.cpp: 
std::cout << engine_info()  -->  misc.cpp  --> console.log "To WebAssembly by MaLa"
wasm_uci_execute  -->  Bitboards.init();  --> console.log "Hello Numb: After Square Distance"
Bitboards.init()  --> init_magics(ROOK, RookTable, Magics)
```

Adding C++ debbuging lines of code:

Chrome reports:
```sh
To WebAssembly by MaLa
Hello Numb: After Square Distance
Hello Numb: Into magics, PieceType ...4: Rook
Aborted(Stack overflow! Stack cookie has been overwritten at 0x12a87e40, expected hex dwords 0x89BACDFE and 0x2135467, but received 0x40808080 0x00000000)
```

Firefox reports:
```sh
To WebAssembly by MaLa
Hello Numb: After Square Distance
================================================================= sf18.js:92:69
==42==ERROR: AddressSanitizer: global-buffer-overflow on address 0x12a83a04 at pc 0x80000180 bp 0x12a839c0 sp 0x12a839cc
WRITE of size 4 at 0x12a83a04 thread T0
    <empty stack>
<empty string>
0x12a83a04 is located 28 bytes before global variable 'buf' defined in '../../../system/lib/libc/musl/src/stdio/stdin.c' (0x12a83a20) of size 1032
0x12a83a04 is located 0 bytes after global variable 'mbrtowc.internal_state' defined in '../../../system/lib/libc/musl/src/multibyte/mbrtowc.c' (0x12a83a00) of size 4
SUMMARY: AddressSanitizer: global-buffer-overflow
```

In types.h:
```C++
// clang-format off
enum PieceType : std::uint8_t {
    NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    ALL_PIECES = 0,
    PIECE_TYPE_NB = 8
};
// clang-format on
```

Google: <br>
In C++, an array declared within a `clang-format-off` block is accessed exactly like any other array. clang-format is a source code formatter, it only affects how your code looks on screen, not how it compiles or runs.


bitboard.cpp around line 172:
```C++
if (size==230) b = 0; // MaLa Debbugging  --> This is the key. If I do that the program runs.
```

So, if we remove this line, we must find what happens after size == 236 when:

```sh
MaLa debugging: Occupancy 
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 8
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 7
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 6
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 5
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 4
+---+---+---+---+---+---+---+---+
| X |   |   |   |   |   |   |   | 3
+---+---+---+---+---+---+---+---+
| X |   |   |   |   |   |   |   | 2
+---+---+---+---+---+---+---+---+
|   |   |   | X | X |   | X |   | 1
+---+---+---+---+---+---+---+---+
  a   b   c   d   e   f   g   h

MaLa debugging: Sliding Attacks 
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 8
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 7
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 6
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 5
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 4
+---+---+---+---+---+---+---+---+
|   |   |   |   |   |   |   |   | 3
+---+---+---+---+---+---+---+---+
| X |   |   |   |   |   |   |   | 2
+---+---+---+---+---+---+---+---+
|   | X | X | X |   |   |   |   | 1
+---+---+---+---+---+---+---+---+
  a   b   c   d   e   f   g   h
```







#### nnue/layers/../simd.h:49:20: error: unknown type name '__m512i' Error

WebAssembly does not support 512 bytes operations.<br>
Read [Using SIMD with WebAssembly](https://emscripten.org/docs/porting/simd.html)<br>
Compile adding -msimd128 for WebAssembly


#### em++: warning: export name is not a valid JS symbol - Use `Module` or `wasmExports` to access this symbol [-Wjs-compiler]

```sh
em++ --clear-cache
```