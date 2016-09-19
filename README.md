# *RSIA*: *R*ate based *S*tatic *I*mpact *A*nalysis for robotic systems

Impact set generator for ROS based systems implemented in C++. Tool extracts and uses the message transfer rate information between two components to reduce the impact set size. 

- Free software: MIT License

## Installation
- Dependencies
	- LLVM (version 3.9.0, requires to be built from source code)([link](http://llvm.org/releases/download.html#3.9.0))
	- Clang (version 3.9.0, requires to be built from source code)([link](http://llvm.org/releases/download.html#3.9.0))
	- Pugi-XML ([link](http://pugixml.org/))
	- Yaml-CPP ([link](https://github.com/jbeder/yaml-cpp))
	- graphviz ([link](http://www.graphviz.org/))
- To Install **Dependency Analysis (DA)** tool [click here](installation/install_DA.md)
- To Install **Impact Analysis (IA)** tool [click here](installation/install_IA.md)
- To Install **Helper Tools (HTs)** [click here](installation/install_HTs.md)

## Examples

You can run examples by checking out:

	git clone https://github.com/unl-nimbus-lab/rsia.git 


## Features

#### Dependency Analysis Tool
- Generates system dependency graph from source code
- Annotation of graph edges with rate-independent labels

#### Impact Analysis Tool
- DFS based component reachability analysis starting at a changed node
- Configurable stopping conditions for graph exploration 
- Simulation of traditional IA techniques

#### Helper Tools

- Parsing of ROS based `.launch` files 
- One click build of whole system


## Credits

[NIMBUS Lab](nimbus.unl.edu) at the University of Nebraska, Lincoln.

This work was partially supported by NSF under awards #1526652 and #1638099 and USDA-NIAF #2013-67021-20947.
