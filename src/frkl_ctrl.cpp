#include <string>
#include <std_msgs/String.h>
#include <ros/ros.h>

#include <std_msgs/Float32.h>
#include <tf/tf.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <nav_msgs/Odometry.h>
#include <turtlebot3_msgs/Sound.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <algorithm>
#include <std_msgs/Bool.h>

int MODE_IDLE = 0;
int MODE_GO_SIMPLE = 1;
int MODE_GO_ADVANCED = 2;

ros::Publisher pub_dest;

void ctrl_Callback(const std_msgs::String msg){
    const char * msg_char = msg.data.c_str();
    std::string str(msg_char);

    ROS_INFO("callback (%s)", msg_char);

}

void sendGO(float pX, float pY, float pTheta, int pMODE){
    geometry_msgs::Pose2D pose2D;
    pose2D.x = pX;
    pose2D.y = pY;
    pose2D.theta = pTheta;

    if(pMODE == MODE_IDLE){

        pub_dest.publish(pose2D);
    }else if(pMODE == MODE_GO_SIMPLE){

    }else if(pMODE == MODE_GO_ADVANCED){

    }else{
        ROS_INFO("ERROR : UNKNOWN CMD SENT");
    }
}

int main(int argc, char** argv) {

    ros::init(argc, argv, "frkl_ctrl");
    ros::NodeHandle n;

    ros::Subscriber ctrl_sub = n.subscribe("ctrl", 1, ctrl_Callback);
    pub_dest = n.advertise<turtlebot3_msgs::Sound>("cmd_vel", 1); //TODO : remove

    ros::Rate loop_rate(30);

    while(1){
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
