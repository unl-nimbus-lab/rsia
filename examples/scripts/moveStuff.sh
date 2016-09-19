#!/bin/bash

cd build/$1

cp model.log /home/nishant/ImpactAnalysis/phaseTwo/test/model.log
cp timers.log /home/nishant/ImpactAnalysis/phaseTwo/test/timers.log
cp publishers.log /home/nishant/ImpactAnalysis/phaseTwo/test/publishers.log
cp subscribers.log /home/nishant/ImpactAnalysis/phaseTwo/test/subscribers.log
rm /home/nishant/ImpactAnalysis/phaseTwo/test/node.log


cp model.log $2/$3/log/model.log
cp timers.log $2/$3/log/timers.log
cp publishers.log $2/$3/log/publishers.log
cp subscribers.log $2/$3/log/subscribers.log
