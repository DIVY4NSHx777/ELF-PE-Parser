# Lightweight PE and LEF parsers (C++17)

This project contains lightweight readers for:
- PE (Portable Executable): extracts DOS header, NT signature, COFF header, and section headers.
- LEF (Library Exchange Format): extracts LAYER names and MACROs with their PIN names.

Target: C++17
Build: CMake (uses FetchContent to download Catch2 for tests)

Quick build and test:

```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .
ctest --output-on-failure
# or run the CLI:
./lef_pe_cli path/to/file.lef
./lef_pe_cli path/to/file.exe
```

Notes:
- Parsers are intentionally lightweight for quick inspection, not full-spec implementations.
- LEF parser handles a small subset of the syntax (LAYER, MACRO, PIN).
- PE parser reads headers and section table; it validates MZ and PE signatures.