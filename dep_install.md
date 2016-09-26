# Dependency Installation for RSIA

**Note:** This guide was written on *Ubuntu 16.04* using *BASH* terminal environment. 

### Installing LLVM and CLANG

Before we move forward with this guide, move to the folder where you wish to install llvm and clang. Note: We will make 2 folders `llvm` and `build_llvm` in the said directory. 

Once you are in the target directory please export the current directory as `llvm_src_dir` 
**For example**, to export the directory `/home/nishant/rsia_test` as base directory:

    export llvm_src_dir='/home/nishant/rsia_test'

Now, we will download and build both llvm and clang: 

#### Retrive llvm and clang

* Move to the base directory.
`cd $llvm_src_dir`
* Download *llvm* into the base directory.
`wget "http://llvm.org/releases/3.9.0/llvm-3.9.0.src.tar.xz"`
* Extract the downloaded src for llvm. 
`tar -xvf llvm-3.9.0.src.tar.xz`
* Rename *llvm* folder for easy access (You can choose to skip this step but from now onwards we will use the folder name llvm instead of llvm-3.9.0.src)
`mv llvm-3.9.0.src llvm`
* Download *Clang* into `$llvm_src_dir/llvm/tools/` directory.

        cd $llvm_src_dir/llvm/tools
        wget "http://llvm.org/releases/3.9.0/cfe-3.9.0.src.tar.xz" 

* Extract clang in the same folder.
`tar -xvf cfe-3.9.0.src.tar.xz`
* Rename *clang* folder for easy access (You can choose to skip this step but from now onwards we will use the folder name clang instead of cfe-3.9.0.src)
`mv cfe-3.9.0.src clang`

#### Build llvm and clang

* Move to the base directory
`cd $llvm_src_dir`
* Make a new directory `build_llvm`.
`mkdir build_llvm`
* Export LLVM and Clang cmake files in this directory using (Note: We set the build mode to release)
        
        cd $llvm_src_dir/build_llvm
        cmake -G "Unix Makefiles" ../llvm -DCMAKE_BUILD_TYPE=Release

* Compile llvm and clang
`make`

