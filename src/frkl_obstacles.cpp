#include <string>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <iostream>
#include <algorithm>
#include <vector>

ros::Publisher ctrl_pub;

float LINEAR_VEL = 0.22;
float STOP_DISTANCE = 0.2;
float LIDAR_ERROR = 0.05;
float SAFE_STOP_DISTANCE = STOP_DISTANCE + LIDAR_ERROR;
float minimum = 3.5;



void get_scan(const sensor_msgs::LaserScan scan){

  std::vector<float> scan_filter(0);

  for(int i = 0; i < scan.ranges.size(); i++){
    scan_filter.push_back(scan.ranges[i]);
  }

	for (int i = 0; i < scan_filter.size() ; ++i){
    if (isinf(scan_filter[i])){
      scan_filter[i] = 3.5;
		}else if (isnan(scan_filter[i])){
      scan_filter[i] = 3.5;
		}
  }

  minimum = 3.5;
	for (int i =0; i < scan_filter.size(); i++){
		if(scan_filter[i] <= minimum && scan_filter[i] >= 0.1){
			minimum = scan_filter[i];
		}
	}
}



int main(int argc, char** argv) {

    ros::init(argc, argv, "frkl_obstacles");
    ros::NodeHandle n;

    ctrl_pub = n.advertise<std_msgs::Bool>("destination/stop", 1);
    ros::Subscriber scanSub=n.subscribe<sensor_msgs::LaserScan>("scan",1,get_scan);

    ros::Rate loop_rate(10);


	  while (ros::ok())
	  {
        std_msgs::Bool col_msg;
      	if (minimum <= 2*SAFE_STOP_DISTANCE){

          col_msg.data = true;
          ctrl_pub.publish(col_msg);
      	  ROS_INFO("Distance de l'obstacle : %f", minimum);
      	}else{
          col_msg.data = false;
          ctrl_pub.publish(col_msg);
      	  ROS_INFO("Pas d'obstacle, le plus près : %f", minimum);
        }


      	ros::spinOnce();
        loop_rate.sleep();
	}



    return 0;
}
