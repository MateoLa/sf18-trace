/*
let memory = new WebAssembly.Memory({ initial: 512, maximum: 1024});
Module["memory"] = memory;

Module["print"] = (text) => { self.postMessage(text) };

Module["printErr"] = (err) => { console.error("MaLa C++ error: ", err); };

Module["onRuntimeInitialized"] = () => { console.log('Module loaded: ', Module); }
*/

Module["terminate"] = () => { PThread.terminateAllThreads(); };
