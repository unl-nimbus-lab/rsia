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

ros::Publisher base_planner_pub;
ros::Subscriber base_planner_sub;

void callback_subscriber(const std_msgs::Bool::ConstPtr& msg)
{
	//some processing
}

void callback_timer(const ros::TimerEvent&)
{
	std_msgs::Bool msg;
	//some processing
	base_planner_pub.publish(msg); 
}

int main(int argc, char **argv)
{
	ros::init(argc, argv, "base_controller");
	ros::NodeHandle n;
	
	base_planner_pub = n.advertise<std_msgs::Bool>("/base_planner",1000);
	base_planner_sub = n.subscribe("/sensor", 1000, callback_subscriber);

	//ros timer will ensure timly execution of the function callback_timer
	ros::Timer timer1 = n.createTimer(ros::Duration(0.1), callback_timer);

	//to wait for incoming messages and call the callback function as messages arrive.
	ros::spin();

	return 0;
}
