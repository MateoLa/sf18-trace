arch = wasm
bits = 64
comp = emscripten
COMP = emscripten
EXE = ../server/sf18.js


ifneq ($(KERNEL),Darwin)
	CXX=em++
else
	CXX=emcc
endif

CXXFLAGS += -flto -DUSE_POPCNT

# Build options
minify_js = no
assertion = yes
multithread = no
wasm_simd = yes
embedded_nnue = yes
debug = yes

VPATH += :emscripten:../server

LDFLAGS += -flto
LDFLAGS += --pre-js emscripten/pre.js
LDFLAGS += --preload-file nn-c288c895ea92.nnue@nn-c288c895ea92.nnue
LDFLAGS += --preload-file nn-37f18f62d772.nnue@nn-37f18f62d772.nnue
LDFLAGS += -s ENVIRONMENT="web,worker" -s MODULARIZE=1 -s EXPORT_ES6=1 -s EXPORT_NAME="Stockfish"
LDFLAGS += -s STACK_SIZE=128KB
LDFLAGS += -s ALLOW_MEMORY_GROWTH=1 -s INITIAL_MEMORY=256MB -s MAXIMUM_MEMORY=2GB
LDFLAGS += -s FORCE_FILESYSTEM=1 -s EXPORTED_RUNTIME_METHODS="['FS', 'cwrap']"
# LDFLAGS += -s EXPORTED_FUNCTIONS="['_uci_step', '_create_instance', '_call_add']"
LDFLAGS += --bind
LDFLAGS += -s EXIT_RUNTIME=0
LDFLAGS += -s ALLOW_UNIMPLEMENTED_SYSCALLS


ifeq ($(minify_js),yes)
	LDFLAGS += --closure 1
else
	LDFLAGS += --closure 0 --minify 0
endif

ifeq ($(assertion),yes)
	LDFLAGS += -s ASSERTIONS=1
else
	LDFLAGS += -s ASSERTIONS=0
endif

ifeq ($(multithread),yes)
	CXXFLAGS += -pthread
	LDFLAGS += -s MALLOC=mimalloc
	LDFLAGS += -s USE_PTHREADS=1 -s SHARED_MEMORY=1 -s PROXY_TO_PTHREAD=1
	LDFLAGS += -s PTHREAD_POOL_SIZE=navigator.hardwareConcurrency-1
else
	CXXFLAGS += -DSINGLE_THREAD
endif
# -s INITIAL_MEMORY / -s MAXIMUM_MEMORY Must be equal for shared memory

ifeq ($(wasm_simd),yes)
	CXXFLAGS += -DUSE_WASM_SIMD -msimd128
#	SRCS += emscripten/wasm_simd.cpp
endif

ifeq ($(embedded_nnue),no)
	CXXFLAGS += -DNNUE_EMBEDDING_OFF
endif

ifeq ($(debug),yes)
	LDFLAGS += --emit-symbol-map
	ifeq ($(COMP),emscripten)
		LDFLAGS += -s SAFE_HEAP=1
#		LDFLAGS += -s PTHREADS_DEBUG=1
		LDFLAGS += -gsource-map=inline --source-map-base http://localhost:6931/ -fsanitize=undefined
#		LDFLAGS += -gsource-map=inline --source-map-base http://localhost:6931/ -fsanitize=undefined,address
	endif
endif


EM_EMSCRIPTEN = $(shell emsdk list | grep INSTALLED | head -n 1 | awk '{ print $$1 }')

EXTRACXXFLAGS += -DEM_EMSCRIPTEN=$(EM_EMSCRIPTEN)

EM_ARTIFACTS += ../server/sf18.js ../server/sf18.wasm ../server/sf18.js.symbols ../server/sf18.wasm.map ../server/sf18.data

# ToDo --> You need to bench a js file from console. May be calling node?
wasm-profile:
ifeq ($(COMP),emscripten)
EXTRALDFLAGS='--coverage'
endif

clean: wasm-clean
wasm-clean:
	@rm -f $(EM_ARTIFACTS)


# Source Mapping
# g4 (deprecated) => -gsource-map
# If the browser cannot find your source files, specify the location using --source-map-base [URL_OR_PATH].
# 6931: emrun port
# optionally: -gsource-map=inline
