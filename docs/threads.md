### Threads

C++ threads map directly to kernel-level threads on almost all modern operating systems.

In a default Emscripten build, the main application thread is exactly the same as the borwser's main UI thread. This behavior changes if you compile your code using specific multithreading flags. <br>
If you complile your project with the Emscripten linker flag `-sPROXY_TO_PTHREAD` your C++ main() function is no longer run on the browser's main UI thread. Emscripten automatically spawns a separate Web Worker behind the scenes.

When using multithreading (-pthread) you can verify which thread you are currently executing on: <br>
  * emscripten_is_main_browser_thread() - returns true if the code is executing on the browser's main UI thread <br>
  * emscripten_is_main_runtime_thread() - returns true if the code is executing on the thread executing main()

When the main() function exits in a multithreaded Emscripten application, by default, if EXIT_RUNTIME=1, completing main() will shut down the entire WebAssembly runtime (aggressively terminating all active background pthreads). If you want lyour background threads to continue running after main() completes, you must explicitly instruct Emscripten to keep the runtime alive compiling with `sEXIT_RUNTIME=0`.



```C++
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool ready = false; // The shared state variable

void worker_thread() {
    std::unique_lock<std::mutex> lock(mtx);
    
    // This blocks if ready is false. 
    // It returns ONLY when 'ready == true' AND the lock is re-acquired.
    cv.wait(lock, []{ return ready; });
    
    std::cout << "Condition is true, processing data...\n";
}

void publisher_thread() {
    {
        std::lock_guard<std::mutex> lock(mtx);
        ready = true; // Set the condition to true
    }
    cv.notify_one(); // Wake up the waiting thread
}

int main() {
    std::thread worker(worker_thread);
    std::thread publisher(publisher_thread);
    worker.join();
    publisher.join();
    return 0;
}
```

#### sf18-trace C++ ./stockfish debugging

Stockfish 18 by the Stockfish developers (see AUTHORS file)
To WebAssembly by MaLa

./stockfish
MaLa debugging: After Square Distance
MaLa debugging: Into magics, PieceType: Rook
MaLa debugging: Out of magics
MaLa debugging: Sparcing Init Magic Calls
MaLa debugging: Into magics, PieceType: Bishop
MaLa debugging: Out of magics
MaLa debug: Init done
MaLa debug: ./stockfish
MaLa debugging: resizing threads
MaLa debugging: requested - 1
MaLa debugging: treads nr - 0
MaLa debugging: manager - 0x57bc85c2e080
MaLa debugging: thread Id - 0
MaLa debugging: counts - 0
MaLa debugging: threads per node - 1
MaLa debug: Into the Thread
MaLa debug: Theads - 1
MaLa debug: numaN - 0
MaLa debug: Total Numa Count - 1
MaLa debug: exit state: 0
MaLa debug: waiting for search finished
MaLa debug: searching boolean - 1
MaLa: I'm parked. I have no work to do
Now Worker thread is processing data...
MaLa debug: before job
MaLa: Who call me?
MaLa debug: job done
MaLa debug: waiting for search finished
MaLa debug: searching boolean - 1
MaLa: In the idle loop I could exit - 0
MaLa: In idle loop I'm going to run the JOB
MaLa: I'm parked. I have no work to do
Now Worker thread is processing data...
MaLa debugging: treads nr - 1
MaLa: Who call me?
MaLa debug: waiting for search finished
MaLa debug: searching boolean - 1
MaLa: In the idle loop I could exit - 0
MaLa: In idle loop I'm going to run the JOB
MaLa: I'm parked. I have no work to do
Now Worker thread is processing data...
MaLa debug: waiting for search finished
MaLa debug: searching boolean - 0
Now Worker thread is processing data...
MaLa debug: waiting for search finished
MaLa debug: searching boolean - 0
Now Worker thread is processing data...
MaLa: Who call me?
MaLa debug: waiting for search finished
MaLa debug: searching boolean - 1
MaLa: In the idle loop I could exit - 0
MaLa: In idle loop I'm going to run the JOB
MaLa: I'm parked. I have no work to do
Now Worker thread is processing data...
MaLa debugging: threads resized
MaLa debugging: the command stream is: 


#### Threads idle_loop()

Like Stockfish stands, Constructor launches the thread and waits until it goes to sleep in idle_loop().

```C++
Thread::Thread(Search::SharedState&                    sharedState,
               std::unique_ptr<Search::ISearchManager> sm,
               size_t                                  n,
               size_t                                  numaN,
               size_t                                  totalNumaCount,
               OptionalThreadToNumaNodeBinder          binder) :
    idx(n),
    idxInNuma(numaN),
    totalNuma(totalNumaCount),
    nthreads(sharedState.options["Threads"]),
    stdThread(&Thread::idle_loop, this)    // --> idle_loop is called here.
```