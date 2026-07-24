<div align="center">

<img src=/assets/stockfish_128x128.png></img>

<h3>Stockfish Wasm</h3>

<p>Stockfish is a free and powerful UCI chess engine. It analyzes chess positions and calculates optimal moves. <br>
Here we complile it for WebAssembly</p>

</div>


### Usage

```sh
cd server
emrun sf.html --no_emrun_detect
```

emrun is an Emscripten local web server and test tool.

Enter some UCI commands.


#### UCI - Universal Chess Interface

Is a command line protocol. You will need to write UCI commands to stdin and listen to stdout. For example:

```sh
uci
isready
position startpos
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
position fen 4r1k1/r1q2ppp/ppp2n2/4P3/5Rb1/1N1BQ3/PPP3PP/R5K1 w - - 1 17
# start the calculation
go depth 20  # by depth
go movetime 5000  # by time (calculate for 5 seconds)
quit
```


#### WebAssembly 

Wasm is a binary instruction format for a stack-based virtual machine. The code could be run in modern browsers to execute compiled code (C++, Rust, Go, etc.) over the virtual machine.

WebAssembly is designed to complement and run alongside JavaScript, sharing functionality between them.


#### Stockfish Source Code Modifications

Stockfish is written in C++ to maximize speed execution. The code has been optimized for certain HW architectures but compilation for web browsers has not been taken into account. We barely modified the source code to achieve this.

Broadly speaking, Stockfish is state machine that holds its state in a UCI::Engine class. It returns a chess position evaluation after each game movement. In C++ it runs in a `uci->loop()` function waiting for the user input which is readed througth the `std::cin` console or the `getline(std::cin, cmd)` library. In browsers, we can initialize the engine and execute one loop step for each movement.

In WebAssembly, both the loop and the I/O method blocks the main browser thread. Let's modify them.

Added or modified files:
```sh
src/Makefile
src/main.cpp
src/misc.cpp
src/uci.h
src/uci.cpp
src/emscripten  # directory
```

In "src/Makefile" we consider a new architecture and compiler: "wasm" and "emscripten".

In browsers, we can avoid the loop and execute one loop step for each movement or uci command. <br>
In main.cpp, the uci->loop():

```C++
    do
    {
        if (cli.argc == 1 && !getline(std::cin, cmd))  cmd = "quit"; // Wait for an input or an end-of-file (EOF) indication
        ...
    } while (token != "quit" && cli.argc == 1);  // The command-line arguments are one-shot
```

We avoid the "getline(std::cin, cmd)" usage and creates a `uci_step()` function that can be called form JS.


##### The UCIEngine class

We need to export the UCIEngine class to JS. <br>
The argument `char* argv[]` (or char** argv) is a pointer to a pointer which is difficult to bind to JS.

With wasmUCI() we try to wrapp the UCIEngine class mapping the double pointer argument to an array. <br>
This modification is very delicate and must be done with care because the engine can fail.

The "char* argv[]" is used to define a CommandLine class that implement the `get_binary_directory()` function which is used to load the nnue networks (from the argv[0] = "path"), that is `binaryDirectory = argv0`.

In WebAssembly path = `./this.programm` while in C++ path = `./stockfish`. <br>
Values loaded when running main() without arguments.

In C++ workingDirectory = ~/path_to_your_stockfish/src <br>
In C++ binaryDirectory = ~/path_to_your_stockfish/src/

In wasm workingDirectory = / <br>
In wasm binaryDirectory = // 

This is because `CommandLine::get_working_directory()` is based on `getcwd`. In WebAssembly traditional filesystem paths are generally unsupported. By default, there is no root directory and the sandbox relies on simulated in-memory filesystems.

2 solutions can be applied: <br>
    * Mount a filesystem - initialize a filesystem backend like IndexedDB or NodeFS or pre-polulate memory using `--preload-file` <br>
    * Use Emscripten File System API - instead of getcwd use `FS.cwd()`

"getcwd" is a C++ POSIX api that queries the system's environment while "FS.cwd" stays locked to the Emscripten root. <br>
We could try to standarize path resolution between C++ and Emscripten but first we going to solve this problem using "preload-file".

When compiling, the NNUE files are downloaded from the network through the net.sh script which uses the file names defined in "evaluate.h".

```sh
LDFLAGS += --preload-file nn-c288c895ea92.nnue@nn-c288c895ea92.nnue
LDFLAGS += --preload-file nn-37f18f62d772.nnue@nn-37f18f62d772.nnue
```

In Makefile, the `--preload-file` flag package the files during compilation so they are available in the virtual cwd of the browser (static packaging). The C++ code can then immediately access the file at the path given in the flag using standard C file APIs.


#### Prerequisites

Install GCC/g++ compilers required to compile C/C++ programs in Linux

```sh
sudo apt install build-essential
```

Install or Update Emscripten

```sh
git clone https://github.com/emscripten-core/emsdk.git  # download Emscripten if you haven't already

cd emsdk
./emsdk update  # update Emscripten is you already have it installed
./emsdk install latest
./emsdk activate latest  # set configuration files
source ./emsdk_env.sh  # set Emscripten into your current console

emcc -v
emcc --version
```

#### Build sf18-wasm

Build options can be set in /src/emscripten/Makefile
If you use the "minify_js" option, the version is compiled with warnings.

```sh
cd src
make ARCH=wasm build -j
```

Download the Neural Network. This command prepares the network to be embedded in the binary.
```sh
make net
```

If you whant to delete all your outputs:
```sh
make ARCH=wasm clean
```


#### Debugging

When trying to compile our own version of Stockfish WebAssembly we face many errors which we summarize [here](/docs/debugging.md).

Notice that if you do not override the Module["stdin"] function, the window.prompt bound to std::cin is triggered, which shows that it is used somewhere in the Stockfish project.


#### Test C++ Stockfish through the console

Download a compiled [relese of stockfish]('https://github.com/official-stockfish/Stockfish/releases/download/sf_18/stockfish-ubuntu-x86-64-vnni512.tar') for a specific architecture.

Or compile stockfish by yourself `make build ARCH=x86-64-avx2 > build.log 2>&1` generating the executable file. Choose the right ARCH.

Execute Stockfich `./stockfish-ubuntu-x86-64-vnni512` or `./stockfish`

Stockfish UCI commands:

```sh
./stockfish  # execute stockfish
uci
isready  # readyok
setoption name UCI_AnalyseMode value true
setoption name Analysis Contempt value Off
setoption name Threads value 32
setoption name Hash value 1024
position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
position fen 4r1k1/r1q2ppp/ppp2n2/4P3/5Rb1/1N1BQ3/PPP3PP/R5K1 w - - 1 17
# start the calculation
go depth 20  # by depth
go movetime 5000  # by time (calculate for 5 seconds)
quit
```

#### References

To understand Stockfish: <br>
[Bitboards](/docs/bitboards.md)
[Bitboards in Chess](/docs/bitboards_in_chess.md)
[Debugging](/docs/debugging.md)
[Threads](/docs/threads.md)


### Acknowledgements

Thanks to the [Stockfish](https://github.com/official-stockfish/Stockfish) team and all its contributors.

Some WebAssembly compilation ideas are based on [Hiroshi Ogawa](https://github.com/hi-ogawa/Stockfish) work and the wasm branch of [Pikafish](https://github.com/official-pikafish/Pikafish)

