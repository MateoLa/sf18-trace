import xx from "./sf18.js"

let memory = new WebAssembly.Memory({ initial: 256, maximum: 1024});

let Module = {
    memory: memory,
    print: (text) => { self.postMessage(text) },
    printErr: (err) => { console.warn("MaLa C++ error: ", err); },
    onRuntimeInitialized: () => { console.log('Module loaded: ', Module); },
};

const sf = await xx(Module);
let wasm_uci = sf.cwrap('wasm_uci', null, ['string']);


self.onmessage = (e) => {
    console.log("MaLa WORKER - cmd: ", e.data);
    wasm_uci(e.data);
}
