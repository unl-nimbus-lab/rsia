# roslaunch_reader

Launch file parser tool for ROS based systems. We use *[pugixml](http://pugixml.org/)* library to parse the xml launch files and then *[yaml-cpp](https://github.com/jbeder/yaml-cpp)* library to dump yaml based output of the nodes that are being launched via the launch file. 

- System Builder helper tool uses this output to compile whole system at once using minimal user input.

#### Features:
* Includes other included files using `<include>` tag
* Resolves `$(<argument>)` tags, currently we support
	- arg
	- env
	- optenv
	- anon
* Adds workspace to the node information 
* Resolves namespace of the nodes 
* Resolves `if`, `unless` attribute arguments to properly launch nodes only if they will be launched via *roslaunch* 

**NOTE:** We ignore `<rosparam>`, and `<machine>` tags as they are not relevent to us. We use `roslaunch` to get information on ros parameters. 

#### Installation

* Install *[pugixml](http://pugixml.org/)*  library
* Install *[yaml-cpp](https://github.com/jbeder/yaml-cpp)* library
* make a build directory, run `cmake <path to cmakelist file>`
* run `make` in build directory, that should give you an executable

#### Execute using:
`./roslaunch_reader <top-level launch file> <output_file> <save extended launch file to /tmp/node.xml by passing "save" (optional)>`
