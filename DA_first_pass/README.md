# DA: Dependency Analysis (First Pass)

## Dependency

DA first pass depends on:

- *ROS*
- *LLVM* (ver 3.9.0, built from source) 
- *Clang* (ver 3.9.0, built from source)

## Pre-Installation Check

Once you have installed Clang (ver 3.9.0) via building from source, please export the base directory of the build location to the variable `llvm_base_dir`. Please visit the [dependency installation guide](../dep_install.md), if you are not sure which folder path to export as `llvm_base_dir` 

Also, to export the Clang build directory:

**Permanently** 

```bash
echo export llvm_base_dir='<path to llvm directory>' >> ~/.bashrc
```

**Temporarily (In current active terminal)**

```bash
export llvm_base_dir='<path to llvm directory>'`
```

Also add Clang executable directory to the path by adding `$llvm_base_dir/build_llvm/bin` to the `PATH` variable.

**Permanently**

```bash
echo export PATH=$PATH:$llvm_base_dir/build_llvm/bin >> ~/.bashrc
```
	
**Temporarily (In current active terminal)**

```bash
export PATH=$PATH:$llvm_base_dir/build_llvm/bin
```

To check if clang is installed properly, execute `clang --version` and make sure the output looks something like this.

```bash
clang version 3.9.0 (tags/RELEASE_390/final)
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: $llvm_base_dir/build_llvm/bin
```

## Installation DA Phase One

To install DA first pass, you have to replace/add some files to the clang file system. 

The process is as follows:

* **Add** `ReachableConditions.h`, `ExprScopeAnalysis.h`, and `RosPatterns.h` to folder `$llvm_base_dir/llvm/tools/clang/include/clang/Analysis/Analyses/`

* **Add** `ReachableConditions.cpp`, `ExprScopeAnalysis.cpp`, and `RosPatterns.cpp` to folder `$llvm_base_dir/llvm/tools/clang/lib/Analysis/`

* **Replace**
`$llvm_base_dir/llvm/tools/clang/include/clang/StaticAnalyzer/Checkers/Checkers.td` with `Checkers.td`

* **Replace**
`$llvm_base_dir/llvm/tools/clang/lib/Analysis/CMakeLists.txt` with `CMakeLists.txt`

* **Replace**
`$llvm_base_dir/llvm/tools/clang/lib/StaticAnalyzer/Checkers/DebugCheckers.cpp` with `DebugCheckers.cpp`

Once the above is done, recompile Clang to add the tool to Clang's static analyzer. 

## Execution

Execution of the tool is explained [here](../examples/README.md).

## Advance Users

If you are planning on modifying or developing on top of this tool, we recommend you to generate and look at doxygen documentation first. We have provided a doxygen configuration file with the source code. It will help understand the details of the system in a better manner than directly looking at the source code. Happy Coding!!!