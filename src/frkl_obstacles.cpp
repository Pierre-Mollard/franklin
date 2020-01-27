#include <string>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/LaserScan.h>
#include <iostream>
#include <algorithm>
#include <vector>

ros::Publisher obstacles_pub;
float LINEAR_VEL = 0.22;
float STOP_DISTANCE = 0.2;
float LIDAR_ERROR = 0.05;
float SAFE_STOP_DISTANCE = STOP_DISTANCE + LIDAR_ERROR;
float minimum = 3.5;
    

void get_scan(const sensor_msgs::LaserScan scan){
        
        std::vector<float> scan_filter(0);
       
        int samples = scan.ranges.size() ;
        int samples_view = 1;     
        int left_lidar_samples_ranges;
	int right_lidar_samples_ranges;


        if (samples_view > samples)
        {
		samples_view = samples;
	}

        if (samples_view == 1)
	{
            scan_filter.push_back(scan.ranges[0]);
	}

        else
	{
            left_lidar_samples_ranges =  (samples_view/2 + samples_view%2)*(-1);
            right_lidar_samples_ranges = samples_view/2;

	std::vector<float> left_lidar_samples(0);
	std::vector<float> right_lidar_samples(0);
            
	for (int j = left_lidar_samples_ranges; j < scan.ranges.size() ; j++)
	{
            left_lidar_samples.push_back(scan.ranges[j]);
	}
	for (int k = 0; k < left_lidar_samples_ranges ; k++)
	{
            right_lidar_samples.push_back(scan.ranges[k]);
	}
            
	    int length_tableau=scan_filter.size()+left_lidar_samples.size()+right_lidar_samples.size();
	    //std::vector<float> new_scan_filter[ length_tableau];
	    //new_scan_filter=scan_filter+left_lidar_samples+right_lidar_samples;
	
	    float new_scan_filter[length_tableau];
	    for (int l = 0; l < scan.ranges.size() ; l++)
	    {
            new_scan_filter[l]=scan_filter[l];
	    }
	   for (int m = 0; m< left_lidar_samples.size() ; m++)
	   {
            new_scan_filter[m+scan.ranges.size() ]=left_lidar_samples[m];
	   }
	   for (int p = 0; p< right_lidar_samples.size() ; p++)
	   {
            new_scan_filter[left_lidar_samples.size()+scan.ranges.size() +p]=right_lidar_samples[p];
	   }
}

	for (int i = 0; i < samples_view ; ++i)
	{
            if (isinf(scan_filter[i]))
		{
                scan_filter[i] = 3.5;
		}
            else if (isnan(scan_filter[i]))
		{
                scan_filter[i] = 0;
		}
        }

	for ( int i =0; i<scan_filter.size();i++)
	{
		if(minimum<scan_filter[i])
		{
			minimum=scan_filter[i];
		}
	}

}

    

int main(int argc, char** argv) {
    
    ros::init(argc, argv, "frkl_obstacles");
    ros::NodeHandle n;

    obstacles_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
    ros::Subscriber scanSub=n.subscribe<sensor_msgs::LaserScan>("/scan",1,get_scan);
    
    ros::Rate loop_rate(10);

    
        geometry_msgs::Twist twist;
	int turtlebot_moving = 1;
	float min_distance;


	  while (ros::ok())
	  {
            
	
            

	if (minimum < SAFE_STOP_DISTANCE)
	{
                if (turtlebot_moving==1)
                {
			twist.linear.x = 0.0;
                    	twist.angular.z = 0.0;
			obstacles_pub.publish(twist);
                    	turtlebot_moving = 0;
                    	ROS_INFO("Stop");
		}
           	else
		{
			twist.linear.x = LINEAR_VEL;
			twist.angular.z = 0.0;
			obstacles_pub.publish(twist);
			turtlebot_moving = 1;
                	ROS_INFO("Distance of the obstacle : %f", min_distance); 
		
		}
	}


		ros::spinOnce();
        	loop_rate.sleep();
	}



    return 0;
}


