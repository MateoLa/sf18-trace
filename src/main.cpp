/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2026 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include <memory>

#include "bitboard.h"
#include "misc.h"
#include "position.h"
#include "tune.h"
#include "uci.h"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/bind.h>
#endif

using namespace Stockfish;


// Test if myClass already exists after main() exits.
struct myClass {
    std::string message = "MaLa engine runing.";
    float xpos = 0;

    float add(){
        return xpos += 1;
    }
};

/*
extern "C" {
    EMSCRIPTEN_KEEPALIVE
    MyClass* create_instance() {
        return new MyClass();
    }
    
    EMSCRIPTEN_KEEPALIVE
    float call_add(MyClass* instance) {
        return instance->add();
    }
}
*/

/*
// UCIEngine class wrapper
std::unique_ptr<UCIEngine> wasm_uci(std::string text) {

    std::stringstream ss(text);
    std::string word;
    std::vector<std::string> words;
    words.push_back("");
    while (ss >> word) { words.push_back(word); }
    int argc = words.size();
    char** argv = new char*[argc];
    for (size_t i = 0; i < argc; ++i) { argv[i] = words[i].data(); }
    
    Bitboards::init();
    Position::init();

    std::cout << "MaLa debug: Init done" << std::endl;
    auto uci = std::make_unique<UCIEngine>(argc, argv);

    Tune::init(uci->engine_options());

    return uci;
}
*/


// UCIEngine class wrapper
class wasmUCI {
private:
    UCIEngine* uci;

public:
    wasmUCI() {
        Bitboards::init();
        Position::init();

        std::cout << "MaLa debug: Init done" << std::endl;
        char** argv = new char*[1];
        std::string mala = "./MaLa-sf";
        argv[0] = mala.data();

        auto uci = std::make_unique<UCIEngine>(1, argv);

        Tune::init(uci->engine_options());
    }

    ~wasmUCI() { delete uci; }

    void uci_step(std::string token) { uci->uci_step(token); }
};


int main(int argc, char* argv[]) {
    std::cout << engine_info() << std::endl;

#ifndef __EMSCRIPTEN__

/*
    argv = new char*[1];
    std::string mala = "./MaLa-sf";
    argv[0] = mala.data();
*/

    for (int i = 0; i < argc; i++) { std::cout << argv[i] << std::endl; }
    std::cout << argc << std::endl;

    Bitboards::init();
    Position::init();

    std::cout << "MaLa debug: Init done" << std::endl;
    auto uci = std::make_unique<UCIEngine>(1, argv);

    Tune::init(uci->engine_options());

    uci->loop();
#endif

    std::cout << "MaLa debug: main exit" << std::endl;

    return 0;
}


EMSCRIPTEN_BINDINGS(sf) {
    emscripten::class_<wasmUCI>("wasmUCI")
	.constructor<>()
        .function("uci_step", &wasmUCI::uci_step);


//    emscripten::function("wasm_uci", &wasm_uci, emscripten::allow_raw_pointers());
//    emscripten::function("wasm_step", &wasm_step, emscripten::allow_raw_pointers());

    emscripten::class_<myClass>("myClass")
	.constructor<>()
        .property("xpos", &myClass::xpos)
        .property("message", &myClass::message)
        .function("add", &myClass::add);
}
