TypeSense Stage 2 - DSA upgrade

Files in this patch (keep the folder paths exactly as shown):
- CMakeLists.txt (adds src/typing_algorithms.cpp)
- src/AdaptiveEngine.h / src/AdaptiveEngine.cpp
- src/MainWindow.h / src/MainWindow.cpp
- src/c_ds/exercise_queue.h / src/c_ds/exercise_queue.c
- src/typing_algorithms.h / src/typing_algorithms.cpp (new)

Features added:
1. C FIFO exercise queue integrated into AdaptiveEngine. The queue owns heap-allocated copies of queued Exercise objects; AdaptiveEngine frees pending items and owns the currently served item.
2. Custom max-heap scheduling: selects the next batch of exercises by weak-key frequency score, then the FIFO queue serves them in order.
3. QHash weak-key frequency table.
4. Sliding-window WPM based on the last 10 keystroke timestamps, shown live in the status label.
5. Levenshtein edit distance shown in the completion result.
6. Dijkstra shortest learning path shown in the adaptive information label.
7. Dynamic exercise generation uses bounded duplicate retries and a fallback instead of unbounded recursion.
8. Backspace is processed even after the final target character has been typed.

Apply:
1. Back up your project.
2. Extract this ZIP into the project root, allowing the included files to replace the matching files.
3. Configure/build using your normal Qt 6 environment:
   cmake -S . -B build
   cmake --build build -j
4. Run:
   ./build/TypeSense

Verification note:
The C FIFO queue was compiled with -Wall -Wextra -Werror and passed a small FIFO/size test in the build environment. A full Qt application build could not be run here because Qt 6 development packages are not installed in this environment. Please run the CMake build on your Mac and send any compiler output if it fails.
