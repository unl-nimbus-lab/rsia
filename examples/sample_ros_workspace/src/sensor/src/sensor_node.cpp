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

int main(int argc, char **argv)
{
	ros::init(argc, argv, "sensor");
	ros::NodeHandle n;
	
	//publisher defined on topic "/sensor"	
	ros::Publisher sensor_pub = n.advertise<std_msgs::Bool>("/sensor", 1000);

	ros::Rate loop_rate(10);

	while (ros::ok())
	{
		std_msgs::Bool msg;
		//we don't populate the msg as this is an example.

		//publish call made inside while loop with a ros::Rate based sleep function will ensure the publish rate to be fixed at loop_rate
		sensor_pub.publish(msg);
	
		//sleep function ensuring timely execution of the loop
		loop_rate.sleep();
	}

	return 0;
}
