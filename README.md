[![Unit Tests](https://github.com/alanabrahampkochumon-zqict/peregrine/actions/workflows/unit-tests.yml/badge.svg)](https://github.com/alanabrahampkochumon-zqict/peregrine/actions/workflows/unit-tests.yml)

# Peregrine Memory Manager

High-performance memory allocators and memory manager.

* Arena Allocator
* Stack Allocator
* Pool Allocator
* TLSF Allocator

# Prerequisites

Ensure the following tools are installed and available in your system `PATH`:

- [CMake](https://cmake.org/download/) (3.25+): Build system generator.
- [Git](https://git-scm.com/downloads): Required for `FetchContent` dependency management.
- [Doxygen (Doxywizard)](https://www.doxygen.nl/download.html) (Optional): Required to generate HTML/PDF documentation.
- [Clang-Format](https://releases.llvm.org/download.html): Required for automated code style enforcement.

## Getting Started

### CMake Flags

- **PMM_ASAN**: Enables AddressSanitizer in Debug mode.
- **PMM_STRICT_MODE**: Enables Strict Mode, treating warnings as errors.
- **PMM_DEMO**: Enables the Peregrine Demo application. _TO BE REMOVED_
- **PMM_DOCS**: Enables Peregrine Doxygen generation.
- **PMM_BENCHMARK**: Enables Peregrine benchmarks.
- **PMM_TESTS**: Enables Peregrine unit tests.

### Configuration

Initialize the build system and download dependencies:

`cmake -B build`

### Compilation

Compile the entire project, including tests and benchmarks.

`cmake --build build`

### Developer Utilities

Run these targets to maintain code quality and documentation.

- Auto-Format: `cmake --build build --target format`

- Generate Docs: cmake `--build build --target docs`

## Usage

## How to install (CMake FetchContent)

1. Use `FetchContent` to pull the library.

```cmake
    FetchContent_Declare(
        peregrine
        GIT_REPOSITORY https://github.com/alan-j316/peregrine.git
        GIT_TAG main
        SYSTEM
)
FetchContent_MakeAvailable(peregrine)
```

2. Link with your target.
    ```cmake
        add_executable(SampleGame)
        target_link_libraries(SampleGame PRIVATE Peregrine::Peregrine)
   ```

### C++ Usage

Allocators can be used directly or through a MemoryManager instance with a single pool shared among all supported
allocator types. Buffer ownership can be handled externally by the user or internally by the allocator.

Allocators can be instantiated with `<Allocator><MemPolicy, TelPolicy, SafeModePolicy, mt::MTPolicy>` or using
specialized
aliases:

- Local<Allocator>: Single-threaded / thread-local allocator.
- SafeLocal<Allocator>: Thread-local allocator with extra nullptr safety checks.
- MT<Allocator>: Thread-safe allocator.
- SafeMT<Allocator>: Thread-safe allocator with extra safety checks.

These specializations are available for all four supported allocators and can be used as-is or by providing an external
buffer.

**Special note for Stacks**: Stack has 2 possible instantiations: `Strict` Stack, which uses a slightly larger memory
footprint to enforce strict LIFO ordering, and `Loose` Stack, which trades safety for a smaller footprint.

#### NOTE

- All allocations are aligned to sizeof(void*) by default, and custom alignments can be specified per call.
- Certain APIs like resizeLast or free are unavailable for stacks in a multithreaded environment. Operations like
  setZero() or clear() are not restricted at compile time, but are strongly discouraged in multithreaded contexts
  outside of a synchronization barrier.

```c++
#include <peregrine/Peregrine.h>

using pmm::constants; // Enables 1_MB


pmm::MemoryManager<> manager{ 1_MB }; // Options 
// OR with an external buffer
const auto buffer = memAlloc(1_MB);
pmm::MemoryManager<pmm::TelPolicy::Enabled, pmm::MemPolicy::External> extManager{ buffer, 1_MB };

// Allocation
auto stack = manager.getSafeStack(1_KB);
auto matrix = stack.malloc(64); // Raw Pointer
auto matrix2 = stack.alloc<Mat4>(1.0f); // Typed allocation.

// Free
stack.mfree(matrix); // Pointer free
stack.free(matrix2); // Free for instantiated classes, will call the destructor for non-trival types.

///----------------- INDIVIDUAL ALLOCATORS ---------------------
pmm::MTPool transformPool(1_MB, 64, 1); // Byte aligned pool. Must specfiy the alignment.
auto actorTransform = transformPool.alloc<TransformComponent>(...);
/// ...CODE...
transformPool.free(actorTransform);


pmm::TLSF< MemPolicy::Internal, TelPolicy::Disabled, SafeModePolicy::Unsafe, mt::MTPolicy::NoMT> dynamicAllocator(1_GB);
std::span<int> elements = dynamicAllocator.allocV<int>(500); // Auto aligned to alignment of type(here int)
/// ...CODE...
dynamicAllocator.freeV(elements);


/// --------------- TELEMETRY -----------------------
const auto& telemetry = dynamicAllocator.getTelemetry();
const auto minUsage = telemetry.getMinPayloadUsage();

pmm::memFree(buffer); // Don't use delete. It will not work.

```

### Testing

The codebase is tested across single-threaded and multithreaded workloads for all allocators on GCC, Clang, and MSVC
across macOS, Windows, and Linux (Ubuntu).

### Looking Ahead

- [ ] Additional telemetry refactoring.
- [ ] Double-sided stack allocator.
- [ ] Naughty Dog's Tag-based allocator.

### Credits

- [Ginger Bill](https://www.gingerbill.org/series/memory-allocation-strategies/)

- [TLSF Allocator](http://www.gii.upv.es/tlsf/)