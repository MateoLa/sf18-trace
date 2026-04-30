#### Emscripten Tracing

Set the `--tracing` option at the command line. It communicates results to an external data collection server. <br>

Features: 
  * Track custom contexts
  * Allocation annotations to track mallocs
  * Logging messages
  * Tasks reporting
  * Event loop reporting
  * Google web tracing framework inter-operability

```C++
#include <emscripten/emscripten.h>
#include <emscripten/bind.h>   // To export functions, values and objects.
#include <emscripten/fetch.h>  // Allows native code to transfer files
#include <emscripten/trace.h>
```

Configure the trace system into the C++ main loop `emscripten_trace_configure("http://127.0.0.1:5000", "STFISH")`. 

Annotate some memory types to label some memory allocations to strings. Thus, you can pick them out in the UI later `emscripten_trace_annotate_address_type(font, "TTY_FONT")`.

Record start and end loop events. Inside any function loop place: 
```C++
emscripten_trace_record_frame_start();
emscripten_trace_record_frame_end();
```

Set contexts to track execute position or segment the program:
```C++
emscripten_trace_enter_context("Initializing Bitboard");
emscripten_trace_exit_context();
``` 

Report memory layout and heap data periodically:
```C++
if(frames % 60 == 0) {
    emscripten_trace_report_memory_layout();
    emscripten_trace_report_off_heap_data();
}
```

I didn't reach to any result trying this way.
