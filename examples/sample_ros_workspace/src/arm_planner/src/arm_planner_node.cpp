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

ros::Publisher arm_planner_pub;
ros::Subscriber arm_planner_sub;

void callback(const std_msgs::Bool::ConstPtr& msg)
{
	//some processing
	arm_planner_pub.publish(msg); 
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "arm_controller");
	ros::NodeHandle n;
	
	arm_planner_pub = n.advertise<std_msgs::Bool>("/arm_planner",1000);
	arm_planner_sub = n.subscribe("/sensor", 1000, callback);

	//to wait for incoming messages and call the callback function as messages arrive.
	ros::spin();

	return 0;
}
