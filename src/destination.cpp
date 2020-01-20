#define _USE_MATH_DEFINES

#include <string>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <nav_msgs/Odometry.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <algorithm>

ros::Publisher cmd_pub;
ros::Publisher info_pub;

float targetX = 0;
float targetY = 0;
float targetT = 0;

float realX = 0;
float realY = 0;
float realT = 0;

float BURGER_MAX_LIN_VEL = 0.22;
float BURGER_MAX_ANG_VEL = 2.84;
float LIN_VEL_STEP_SIZE = 0.01;
float ANG_VEL_STEP_SIZE = 0.1;

float angular_speed = 1;
float linear_speed = 1;

void dest_Callback(const geometry_msgs::Pose2D pose2D){
    ROS_INFO("Demande de destination \n- x=%0.2f\n- y=%0.2f\n- theta=%0.2f", pose2D.x, pose2D.y, pose2D.theta);
    targetX = pose2D.x;
    targetY = pose2D.y;
    targetT = pose2D.theta;

}

void odom_Callback(const nav_msgs::Odometry odom){
  /** CAMERA **/
    realX = odom.pose.pose.position.x;
    realY = odom.pose.pose.position.y;
    realT = odom.pose.pose.orientation.z;
}

int main(int argc, char** argv) {

    ros::init(argc, argv, "destination");
    ros::NodeHandle n;

    cmd_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
    info_pub = n.advertise<std_msgs::Float32>("info_dest", 1);
    ros::Subscriber dest_sub = n.subscribe("destination", 1, dest_Callback);
    ros::Subscriber odom_sub = n.subscribe("odom", 1, odom_Callback);

    ros::Rate loop_rate(30);
    float last_rotation = 0;

    while(1){
        ros::spinOnce();

        double distance = sqrt(pow(targetX - realX, 2) + pow(targetY - realY, 2));
        float rotation = realT;
        while(distance > 0.05){
/*
          float path_angle = atan2(targetY - realY, targetX - realX);
          rotation = realT;

          if(path_angle < -M_PI/4 || path_angle > M_PI/4){
              if(targetY < 0 && realY < targetY){
                path_angle = -2*M_PI + path_angle;
              }else if(targetY >= 0 && targetY < realY){
                path_angle = 2*M_PI + path_angle;
              }
              if(last_rotation > M_PI - 0.1 && rotation <= 0){
                rotation = 2*M_PI + rotation;
              }else if(last_rotation <= -M_PI+0.1 && rotation > 0){
                rotation = -2*M_PI + rotation;
              }
          }
*/
          geometry_msgs::Twist twist;/*
          twist.angular.z = path_angle-realT;

          if(twist.angular.z > 0){
            twist.angular.z = std::min(twist.angular.z, 1.5);
          }else{
            twist.angular.z = std::min(twist.angular.z, -1.5);
          }
*/
          distance = sqrt(pow(targetX - realX, 2) + pow(targetY - realY, 2));
          twist.linear.x = std::min(distance, 0.1);

          last_rotation = rotation;
          cmd_pub.publish(twist);
          ROS_INFO("DISTANCE =%0.2f", distance);

          loop_rate.sleep();
      }
      geometry_msgs::Twist twist;
      twist.linear.x = 0;
      cmd_pub.publish(twist);
    }

    return 0;
}
