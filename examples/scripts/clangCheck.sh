#!/bin/bash

# 1. Ensure that both clang and scan-build point to the compiled versions that you built under clang/llvm. clang should be under "build" subditectory and scan-build should be under llvm/tools/clang/tools/scanbuild. Set the PATH according in .bashrc or .bash_profile. Type "type scan-build" and "type clang" to confirm that you are running right versions.

# 2. Go to the top level of you ROS application (node). run rosmake.
#sudo bash # if required
#rosmake
rm -r build
mkdir build
cd build
#cmake -DCMAKE_CXX_COMPILER=/opt/llvm/tools/clang/tools/scan-build/c++-analyzer -DCMAKE_CC_COMPILER=/opt/llvm/tools/clang/tools/scan-build/ccc-analyzer ..

cmake ../src -DCMAKE_INSTALL_PREFIX=../install -DCATKIN_DEVEL_PREFIX=../devel -DCMAKE_CXX_COMPILER=/opt/build_llvm/libexec/c++-analyzer -DCMAKE_CC_COMPILER=/opt/build_llvm/libexec/ccc-analyzer

/opt/build_llvm/bin/scan-build --use-analyzer=/opt/build_llvm/bin/clang -enable-checker debug.DumpRosPatterns make

#/opt/build_llvm/bin/scan-build --use-analyzer=/opt/build_llvm/bin/clang -enable-checker debug.DumpExprScope make

#scan-build --use-analyzer=/opt/build_llvm/bin/clang -enable-checker debug.DumpCFG make

#scan-build --use-analyzer=/opt/llvm/Release+Asserts/bin/clang -enable-checker debug.DumpReachingConditions -load-plugin ~/pluginDumpSymbolTable.so make
#exit # if sudo done
#This should create a file model.log in the current directory

#cmake ../src -DCMAKE_INSTALL_PREFIX=../install -DCATKIN_DEVEL_PREFIX=../devel -DCMAKE_CXX_COMPILER=/opt/llvm/tools/clang/tools/scan-build/c++-analyzer -DCMAKE_CC_COMPILER=/opt/llvm/tools/clang/tools/scan-build/ccc-analyzer
