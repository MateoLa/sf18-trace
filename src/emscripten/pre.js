/*
// Not needed when this values are set in Makefile.
let memory = new WebAssembly.Memory({ 
    initial: 2048,       // In pages (1 page = 64KB). 2048 = 128MB 
    maximum: 32768,      // 2GB -> 2*(1024*1024*1024)/(64*1024) 
    shared: true
});
Module["memory"] = memory;

Module["print"] = (text) => { self.postMessage(text) };

Module["printErr"] = (err) => { console.warn("MaLa C++ error: ", err); };

Module["onRuntimeInitialized"] = () => { 
    console.log('MaLa Module Loaded');
    Module.wasm_uci = Module.cwrap('wasm_uci', null, ['string']);
}
*/

Module["terminate"] = () => { PThread.terminateAllThreads(); };
