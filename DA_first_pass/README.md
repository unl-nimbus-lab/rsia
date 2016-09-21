# DA: Dependency Analysis (Phase One)

## Dependency
DA Phase one depends on:
- *LLVM* (ver 3.9.0, built from source) 
- *Clang* (ver 3.9.0, built from source)

## Pre-Installation Check

Once you have installed Clang (ver 3.9.0) via building from source, please export the build directory of Clang to a variable `clang_build_dir`.

To export the Clang build directory:

- **Permanently** 

    `echo export clang_build_dir='<path to build directory>' >> ~/.bashrc`
	
- **Temporarily**

    `export clang_build_dir='<path to build directory>'`

Also add Clang executable to the path by adding`$clang_build_dir/bin` to the `path` variable.

To check if clang is installed properly, execute `clang --version` and make sure the output looks something like this.

    clang version 3.9.0 
    Target: x86_64-unknown-linux-gnu
    Thread model: posix
    InstalledDir: <path>

## Installation DA Phase One

To install DA phase, you have to replace/add some files to the clang file system. 

The process is as follows:

* **Add** `ReachableConditions.h`, `ExprScopeAnalysis.h`, and `RosPatterns.h` to folder `<path to llvm source dir>/llvm/tools/clang/include/clang/Analysis/Analyses/`

* **Add** `ReachableConditions.cpp`, `ExprScopeAnalysis.cpp`, and `RosPatterns.cpp` to folder `<path to llvm source dir>/llvm/tools/clang/lib/Analysis/`

* **Replace**
`<path to llvm source dir>/llvm/tools/clang/include/clang/StaticAnalyzer/Checkers/Checkers.td` with `Checkers.td`

* **Replace**
`<path to llvm source dir>/llvm/tools/clang/lib/Analysis/CMakeLists.txt` with `CMakeLists.txt`

* **Replace**
`<path to llvm source dir>/llvm/tools/clang/lib/StaticAnalyzer/Checkers/DebugCheckers.cpp` with `DebugCheckers.cpp`

Once the above is done, recompile Clang to add the tool to Clang's static analyzer. 


## Execution

Execution of the tool is explained [here](tbd).
