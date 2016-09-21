/**
 _   _ _           _                 _           _                _   _ _   _  _
| \ | (_)         | |               | |         | |              | | | | \ | || |
|  \| |_ _ __ ___ | |__  _   _ ___  | |     __ _| |__    ______  | | | |  \| || |
| . ` | | '_ ` _ \| '_ \| | | / __| | |    / _` | '_ \  |______| | | | | . ` || |
| |\  | | | | | | | |_) | |_| \__ \ | |___| (_| | |_) |          | |_| | |\  || |____
\_| \_/_|_| |_| |_|_.__/ \__,_|___/ \_____/\__,_|_.__/            \___/\_| \_/\_____/
*/

/**
 * @author: Nishant Sharma
 */

#include "ros/ros.h"
#include "std_msgs/Bool.h"

#include <sstream>

void callback(const std_msgs::Bool::ConstPtr& msg)
{
	//some processing 
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "arm_controller");
	ros::NodeHandle n;
	
	//this one is just a consumer node
	ros::Subscriber arm_planner_sub = n.subscribe("/arm_planner", 1000, callback);

	//to wait for incoming messages and call the callback function as messages arrive.
	ros::spin();

	return 0;
}
