
`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/ReachableConditions.h /opt/llvm/tools/clang/include/clang/Analysis/Analyses/ReachableConditions.h`

`rm /opt/llvm/tools/clang/include/clang/StaticAnalyzer/Checkers/Checkers.td`

`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/Checkers.td /opt/llvm/tools/clang/include/clang/StaticAnalyzer/Checkers/Checkers.td`

`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/ReachableConditions.cpp /opt/llvm/tools/clang/lib/Analysis/ReachableConditions.cpp`

`rm /opt/llvm/tools/clang/lib/Analysis/CMakeLists.txt`

`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/CMakeLists.txt /opt/llvm/tools/clang/lib/Analysis/CMakeLists.txt`

`rm /opt/llvm/tools/clang/lib/StaticAnalyzer/Checkers/DebugCheckers.cpp`

`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/DebugCheckers.cpp /opt/llvm/tools/clang/lib/StaticAnalyzer/Checkers/DebugCheckers.cpp`


`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/RosPatterns.h /opt/llvm/tools/clang/include/clang/Analysis/Analyses/RosPatterns.h`

`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/RosPatterns.cpp /opt/llvm/tools/clang/lib/Analysis/RosPatterns.cpp`


`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/ExprScopeAnalysis.h /opt/llvm/tools/clang/include/clang/Analysis/Analyses/ExprScopeAnalysis.h`

`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/ExprScopeAnalysis.cpp /opt/llvm/tools/clang/lib/Analysis/ExprScopeAnalysis.cpp`

`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/RosParamAnalysis.h /opt/llvm/tools/clang/include/clang/Analysis/Analyses/RosParamAnalysis.h`

`ln -s /home/nishant/vcs/ImpactAnalysis/checkers/RosParamAnalysis.cpp /opt/llvm/tools/clang/lib/Analysis/RosParamAnalysis.cpp`

