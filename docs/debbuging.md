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
    bitboard.cpp:146:29  --> reference[size] = Bitboards::sliding_attack(pt, s, b);
    bitboard.cpp:79  --> init_magics(ROOK, RookTable, Magics);

Compiling with the option `-fsanitize=undefined,address` the browser reports: `AddressSanitizer: out of memory: allocator is trying to allocate 0xa0100000 bytes` (This is 2685403136 bytes or 2,5 GB).

#### Emscripten Tracing

Set the `--tracing` option at the command line. It communicates results to an external data collection server. <br>

Features: 
  * Track custom contexts
  * Allocation annotations to track mallocs
  * Logging messages
  * Tasks reporting
  * Event loop reporting
  * Google web tracing framework inter-operability

```C++
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>   // To export functions, values and objects.
#include <emscripten/fetch.h>  // Allows native code to transfer files
#include <emscripten/trace.h>
```

Configure the trace system into the C++ main loop `emscripten_trace_configure("http://127.0.0.1:5000", "STFISH")`. 

Annotate some memory types to label some memory allocations to strings. Thus, you can pick them out in the UI later `emscripten_trace_annotate_address_type(font, "TTY_FONT")`.

Record start and end loop events. Inside any function loop place: 
```C++
emscripten_trace_record_frame_start();
emscripten_trace_record_frame_end();
```

Set contexts to track execute position or segment the program:
```C++
emscripten_trace_enter_context("Initializing Bitboard");
emscripten_trace_exit_context();
``` 

Report memory layout and heap data periodically:
```C++
if(frames % 60 == 0) {
    emscripten_trace_report_memory_layout();
    emscripten_trace_report_off_heap_data();
}
```

I didn't reach to any result trying this way.

#### Stockfish Memory access out of bounds Manual Trace

```C++
main.cpp: 
std::cout << engine_info()  -->  misc.cpp  --> console.log "To WebAssembly by MaLa"
wasm_uci_execute  -->  Bitboards.init();  --> console.log "Hello Numb: After Square Distance"
Bitboards.init()  --> init_magics(ROOK, RookTable, Magics)
```

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

Firefox and Chrome behaves differently.<br>
In Chrome the code crash after the line that prints out "Hello Numb: Into magics, PieceType ...4: Rook" <br>
While in FF the code crash before but it points the memory address `0x12a83a04` that the code is trying to access which causes the buffer overflow.

The error sims to be in the line: `init_magics(ROOK, RookTable, Magics)` <br>
ROOK is a constant. <br>
RookTable is an array (buffer) of Bitboards to store rook attacks  -->  Bitboard RookTable[0x19000]; <br>
Magics is a 2 dimensional array that holds all magic bitboards relevant data for a single square -->  Magics[SQUARE_NB][2]; <br>
The call to init_magics is to initialize RookTable and Magics arrays.





#### nnue/layers/../simd.h:49:20: error: unknown type name '__m512i' Error

WebAssembly does not support 512 bytes operations.<br>
Read [Using SIMD with WebAssembly](https://emscripten.org/docs/porting/simd.html)<br>
Compile adding -msimd128 for WebAssembly


#### em++: warning: export name is not a valid JS symbol - Use `Module` or `wasmExports` to access this symbol [-Wjs-compiler]

```sh
em++ --clear-cache
```