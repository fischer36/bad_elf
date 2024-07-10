# bad_elf 
An experimental shared-library toolkit for runtime manipulation of ELF processes.

## Features
- Import address table hooking (Global Offset Table).
- Modify memory mapping permissions e.g. read/write/execute of target.
- Query shared libraries.

## How it works
1. compile.sh runs cargo build to compile the library. In build.rs, instructions compile and statically link src/init.c into the library. init.c conains some FFI buffoonery including the shared-library constructor that jumps to Rust entry function. dummy/dummy.c program is then launched with LD_PRELOAD ensuring the Rust library is loaded before the linker. This setup allows additional static override of an imported function by recreating the signature, which the linker then automatically configures in the GOT.
