/*
if (!Module["preRun"]) Module["preRun"] = [];
Module["preRun"].push(function () { let wasm_uci_execute = Module.cwrap("wasm_uci_execute", "void", []); });
*/

Module["terminate"] = () => { PThread.terminateAllThreads(); };
