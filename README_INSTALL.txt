TypeSense complete source bundle (Stage 2)

This bundle contains a COMPLETE src/ directory, not a partial patch.
It also includes the matching root CMakeLists.txt.

Installation:
1. Back up your existing TypeSense project.
2. Open the project root (the folder containing CMakeLists.txt).
3. Replace the entire existing src folder with this bundle's src folder.
4. Replace the root CMakeLists.txt with the one included here.
5. Configure and build:
   cmake -S . -B build
   cmake --build build -j
6. Run:
   ./build/TypeSense

The bundle preserves the original main.cpp, db_viewer.cpp, TypingEngine,
WebcamHandler, DatabaseManager, and correction stack files, while including
the Stage 2 AdaptiveEngine, queue integration, and typing algorithms.

Verification:
The C queue was independently compiled/tested. A full Qt application build
was not run in the generation environment. Please verify on the Mac with the
Qt 6 environment used by the project.
