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


#### Memory access out of bounds Manual Trace

```C++
main.cpp: 
std::cout << engine_info()  -->  misc.cpp  --> console.log "To WebAssembly by MaLa"
wasm_uci_execute  -->  Bitboards.init();  --> console.log "Hello Numb: After Square Distance"
Bitboards.init()  --> init_magics(ROOK, RookTable, Magics)
```


In bitboard.cpp: <br>
Adding many lines of code like: <br>
`std::cout << "MaLa debugging: Sliding Attacks \n" + Bitboards::pretty(reference[size]) << std::endl;` <br>
I reached a point at the `do{...}while (b);` loop, when `size==236`, at which stockfish and the browser crashed. 

Chrome reports:
```sh
To WebAssembly by MaLa
MaLa debugging: After Square Distance
MaLa debugging: Into magics, PieceType ...4: Rook
Aborted(Stack overflow! Stack cookie has been overwritten at 0x12a87e40, expected hex dwords 0x89BACDFE and 0x2135467, but received 0x40808080 0x00000000)
```

The error does not occurs due to any specific instruction, so I think it's a Busy waiting issue.

The following reinforces this idea. <br>
bitboard.cpp, inside the loop and around line 172:
```C++
if (size==230) b = 0; // MaLa Debbugging: If I do that the program runs.
```

WASM operates in a memory-safe sandboxed environment. Allocate memory from the WASM heap or linear memory space to ensure compatibility.

Also, the emrun server reports:
`[ERROR:base/memory/shared_memory_switch.cc:289] Failed global descriptor lookup: 7` <br>
It indicates a regression in the browser's sandbox brokering, where the browser fails to correctly map shared memory segments into its secure sandbox.

Potential Cause: <br>
Stale Shared Memory: un-detached memory segments can cause lookup failures.

Google: <br>
WebAssembly memory segments ramain un-detached from JavaScript when using SharedArrayBuffer or with resizable ArrayBuffers via toResizableBuffer().<br>
Traditionally, Wasm memory growth detaches existing ArrayBuffer views in JS, invalidating them. The toResizableBuffer() method prevents this detachment. <br>
When WebAssembly.Memory is initialized with a SharedArrayBuffer, the memory is shared between JavaScript and Wasm threads and it does not detach upon growth.

Debuggin with SAFE_HEAP instead of ASan (Address Sanitizer) I get the error: <br>
`Stack overflow detected.  You can try increasing -sSTACK_SIZE (currently set to 65536)`

* Error Summary:
I added the line `LDFLAGS += -s STACK_SIZE=128KB` and the error was resolved.


#### Thread Constructor Failed

The error reported by the browser is: <br>
system_error was thrown in -fno-exceptions mode with error 138 and message "thread constructor failed"

The error stack at the console shows:
```sh
$func919	@	system_error.cpp:364
$func1121	@	thread.h:217
$func1401	@	thread.cpp:240
$func907	@	engine.cpp:151
$func906	@	uci.cpp:68
$__main_argc_argv	@	main.cpp:46
```


#### nnue/layers/../simd.h:49:20: error: unknown type name '__m512i' Error

WebAssembly does not support 512 bytes operations.<br>
Read [Using SIMD with WebAssembly](https://emscripten.org/docs/porting/simd.html)<br>
Compile adding -msimd128 for WebAssembly


#### em++: warning: export name is not a valid JS symbol - Use `Module` or `wasmExports` to access this symbol [-Wjs-compiler]

```sh
em++ --clear-cache
```


#### Lesson 1

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


#### Lesson 2

Emscripten uses WebAssembly's linear memory model: a single contiguous arrayBuffer accessed from both WebAssembly and JavaScript. <br>
The wasmMemory object is a WebAssembly.Memory instance created during initialization. The updateMemoryViews() function creates typed array views over wasmMemory.buffer whenever memory allocated or grown.

```sh
# Address Space (0x0000)  =>  
    Region:                 Config:
Static Data             GLOBAL_BASE (default 1024)          (Global variables, constants, read-only data)
    |
Dynamic Heap            INITIAL_HEAP                        (malloc/new -> free allocations)
    |
Stack Region            STACK_SIZE (default 64KB)   =>      INITIAL_MEMORY (Top of initial memory)
```

* Memory Heap (Pila de memoria)

INITIAL_MEMORY sets the starting size of the memory. <br>
INITIAL_HEAP defines the specific amount of memory available for dynamic allocations (via malloc or new) independently of static data.

The heap is compatible with C/C++ alignment rules. Unaligned reads/writes may work but can be significantly slower. You can use -s SAFE_HEAP=1 during debugging to catch alignment or out-of-bounds issues. Emscripten ASan (Address Sanitizer) does not work with SAFE_HEAP.


#### References

[Memory Management](https://deepwiki.com/emscripten-core/emscripten/4.2-memory-management)