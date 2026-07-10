import xx from "./sf18.js"

let memory = new WebAssembly.Memory({ initial: 256, maximum: 512});

let engine;
let uci, my = null;


let Module = {
    memory: memory,
    print: (text) => { self.postMessage(text) },
    printErr: (err) => { console.warn("MaLa C++ error: ", err); },
    onRuntimeInitialized: function() { console.log('Module loaded: ', Module); }
};

// Initialize the module with your options xx(options)
xx(Module).then((instance) => { 
    console.log("Module fully loaded");
    my = new instance.myClass;
    uci = new instance.wasmUCI();
    console.log(uci);
    engine = instance 
});

self.onmessage = (e) => { engine.one_frame(e.data); }
