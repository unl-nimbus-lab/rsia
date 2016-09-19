#!/bin/bash

cwd=`pwd`
#remove old files
rm /home/nishant/ImpactAnalysis/Simulation/mismatches.txt
#recompile every remap node
cd $cwd/$i/remap
rm ./../../missing.txt

cd $cwd

zk=11


for (( i=$zk	; i<=$zk; i++ ))
do
	reset
	cd $cwd
	echo $i
	name=`cat $i/name`
	echo $name
	#empty compilation folder and then copy materials into it.
   	rm -rf /home/nishant/projects/temp_ws/src/*
   	rm -rf /home/nishant/projects/temp_ws/devel/*
   	rm -rf /home/nishant/projects/temp_ws/build/*
   	cp -r $i/* /home/nishant/projects/temp_ws/src/
   	cp /home/nishant/projects/CMakeLists.txt /home/nishant/projects/temp_ws/src/

   	#compile and run the package
   	cd /home/nishant/projects/temp_ws
   	/home/nishant/ImpactAnalysis/Simulation/clangCheck.sh

   	#take the compiled files and copy them to the phasetwo run location
	/home/nishant/ImpactAnalysis/Simulation/moveStuff.sh $name $cwd $i
	
	cd /home/nishant/ImpactAnalysis/phaseTwo/test
	echo "phase 2 started"
	/home/nishant/ImpactAnalysis/phaseTwo/test/phaseTwo defaults
	
	echo "phase 2 completed: now remap"

	#recompile every remap node
	cd $cwd/$i/remap
	make
	
	cd /home/nishant/ImpactAnalysis/phaseTwo/test
	#remap output to it's proper location and output file directory is passed as parameter
	echo "remapping"
	$cwd/$i/remap/remap "/home/nishant/ImpactAnalysis/GraphTraversal/test/Automatic_Analysis/Robot_Graph_Files/cob/"
done
