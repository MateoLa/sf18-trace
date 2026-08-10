/*
if (!Module["preRun"]) Module["preRun"] = [];

Module["preRun"].push(function () { 
    Module.wasm_uci = Module.ccall("wasm_uci", null, ['string']);
});
*/

let memory = new WebAssembly.Memory({ initial: 512, maximum: 1024});
Module["memory"] = memory;

Module["print"] = (text) => { self.postMessage(text) };

Module["printErr"] = (err) => { console.error("MaLa C++ error: ", err); };

Module["onRuntimeInitialized"] = () => { 
    console.log('Module loaded: ', Module);
    Module.wasm_uci = Module.cwrap('wasm_uci', null, ['string']);
}

Module["terminate"] = () => { PThread.terminateAllThreads(); };

self.onmessage = (e) => { 
    console.log("MaLa WORKER - Calling UCI w/ ", e.data);
    Module.cwrap('wasm_uci', null, ['string'], [e.data]);
}