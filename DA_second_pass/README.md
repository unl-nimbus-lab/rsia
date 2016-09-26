# DA: Dependency Analysis (Second Pass)

## Dependency
DA Phase one depends on:
- *Yaml-Cpp* 

See Dependency installation [guide](/dep_install.md) guide for how to install dependencies.

## Installation

Once you have the dependencies installed, you can install *DA_second_pass* as follows (assumption, our current working directory is DA_second_pass inside where you cloned rsia):

* Make a new folder *build* for compiling the executable

```bash
mkdir build
```
* Export cmake based make files into the newly made build folder
```bash
cd build
cmake ..
```
* Compile the executable
```bash
make
```
## Usuage

To execute the second pass of DA:
```
./DA_second_pass <launchFile>
OR
./DA_second_pass defaults
```
Launch file tells DA some information about the node being parsed and file locations. Here is an example launch configuration file contents with default values:

```yaml
NodeName :Node 
OutputFilename :node.yaml 
InTimers :timers.log 
InSubscribers :subscribers.log 
InPublishers :publishers.log 
InModel :model.log
```

**NOTE:** `defaults` option of *DA_second_pass* expects all the files to be in the same directory from where the executable is being ran from. 
