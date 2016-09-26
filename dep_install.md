# Dependency Installation for RSIA

**Note:** This guide was written on *Ubuntu 16.04* using *BASH* terminal environment. 

## Installing ROS

* To install ROS for **_Ubuntu 16.04_** follow this ([link](http://wiki.ros.org/kinetic/Installation/Ubuntu)).
* To install ROS for **_Ubuntu 14.04_** follow this ([link](http://wiki.ros.org/indigo/Installation/Ubuntu)).


## Installing LLVM and CLANG

Before we move forward with this guide, move to the folder where you wish to install llvm and clang. Note: We will make 2 folders `llvm` and `build_llvm` in the said directory. 

Once you are in the target directory please export the current directory as `llvm_base_dir` 
**For example**, to export the directory `/home/nishant/rsia_test` as base directory:

```bash
export llvm_base_dir='/home/nishant/rsia_test'
```

Now, we will download and build both llvm and clang: 

#### Retrive llvm and clang

* Move to the base directory.

```bash
cd $llvm_base_dir
```

* Download *llvm* into the base directory.

```bash
wget "http://llvm.org/releases/3.9.0/llvm-3.9.0.src.tar.xz"
```

* Extract the downloaded src for llvm. 

```bash
tar -xvf llvm-3.9.0.src.tar.xz
```
* Rename *llvm* folder for easy access (You can choose to skip this step but from now onwards we will use the folder name llvm instead of llvm-3.9.0.src)

```bash
mv llvm-3.9.0.src llvm
```

* Download *Clang* into `$llvm_base_dir/llvm/tools/` directory.

```bash
cd $llvm_base_dir/llvm/tools
wget "http://llvm.org/releases/3.9.0/cfe-3.9.0.src.tar.xz" 
```

* Extract clang in the same folder.

```bash
tar -xvf cfe-3.9.0.src.tar.xz
```
* Rename *clang* folder for easy access (You can choose to skip this step but from now onwards we will use the folder name clang instead of cfe-3.9.0.src)

```bash
mv cfe-3.9.0.src clang
```

#### Build llvm and clang

* Move to the base directory

```bash
cd $llvm_base_dir
```
* Make a new directory `build_llvm`.

```bash
mkdir build_llvm
```

* Export LLVM and Clang cmake files in this directory using (Note: We set the build mode to release)

```bash
cd $llvm_base_dir/build_llvm
cmake -G "Unix Makefiles" ../llvm -DCMAKE_BUILD_TYPE=Release
```

* Compile llvm and clang

```bash
make
```
Wait for *make* to finish. Once finished *LLVM* and *Clang* are successfully built. Verify clang version by running 

```bash
$llvm_base_dir/build_llvm/bin/clang --version
``` 
And it should return this to verify a successful install.

```bash
clang version 3.9.0 
Target: x86_64-unknown-linux-gnu
Thread model: posix
InstalledDir: $llvm_base_dir/build_llvm/bin
```

## Installing yaml-cpp 

Yaml-cpp can be installed in two ways:

1. Through ubuntu repositories using

        sudo apt-get install libyaml-cpp-dev

2. Can be built through sources and installed. Check yaml-cpp's github repository for that. ([Click here](https://github.com/jbeder/yaml-cpp))

## Installing pugi-xml

pugi-xml can be installed in two ways:

1. Through ubuntu repositories using
 
        sudo apt-get install libpugixml-dev

2. Can be built through sources and installed. Check pugi-xml's homepage. ([Click here](https://http://pugixml.org/))

## Installing graphviz

you can install graphviz using

```bash
sudo apt-get install graphviz
``` 
