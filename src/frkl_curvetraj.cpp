#define _USE_MATH_DEFINES

#include <string>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include <ros/ros.h>
#include <tf/tf.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <nav_msgs/Odometry.h>
#include <stdio.h>
#include <math.h>
#include <iostream>
#include <algorithm>

ros::Publisher cmd_pub;
ros::Publisher info_pub;

double targetX = 1;
double targetY = 1;
double targetT = 0;

double realX = 0;
double realY = 0;
double realT = 0;

double initX = 0;
double initY = 0;
double initT = 0;

double BURGER_MAX_LIN_VEL = 0.22; //double BURGER_MAX_LIN_VEL = 0.22;
double BURGER_MAX_ANG_VEL = 1.5; //double BURGER_MAX_ANG_VEL = 2.84;
double LIN_VEL_STEP_SIZE = 0.01;
double ANG_VEL_STEP_SIZE = 0.1;

float angular_speed = 1;
float linear_speed = 1;

const double PI = 3.14159265358979323846;

bool working = false;
int state = 0;
double initial_path_distance = 1;

void dest_Callback(const geometry_msgs::Pose2D pose2D){
    ROS_INFO("Demande de destination \n- x=%0.2f\n- y=%0.2f\n- theta=%0.2f", pose2D.x, pose2D.y, pose2D.theta);
    targetX = pose2D.x;
    targetY = pose2D.y;
    targetT = pose2D.theta * PI/180;
    initX = realX;
    initY = realY;
    initT = realT;
    working = true;
    state = 0;
    initial_path_distance = sqrt((targetX - realX)*(targetX - realX)  + (targetY - realY)*(targetY - realY));
}

void odom_Callback(const nav_msgs::Odometry odom){
  /** TODO : CAMERA : get data from frkl_poscalc node**/

    // linear position
    realX = odom.pose.pose.position.x;
    realY = odom.pose.pose.position.y;

    // quaternion to RPY conversion
    tf::Quaternion q(
        odom.pose.pose.orientation.x,
        odom.pose.pose.orientation.y,
        odom.pose.pose.orientation.z,
        odom.pose.pose.orientation.w);
    tf::Matrix3x3 m(q);
    double roll, pitch, yaw;
    m.getRPY(roll, pitch, yaw);

    // angular position
    realT = yaw;

    //ROS_INFO("LECTURE ODOM \n- %0.2f\n- %0.2f\n- %0.2f", realX, realY, realT);
}

void pubPercentage(float value){
  std_msgs::Float32 msg;
  msg.data = value;
  info_pub.publish(msg);
}

int main(int argc, char** argv) {

    ros::init(argc, argv, "frkl_curvetraj");
    ros::NodeHandle n;

    //pubs
    cmd_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
    info_pub = n.advertise<std_msgs::Float32>("f_info_dest", 1);

    //subs
    ros::Subscriber dest_sub = n.subscribe("destination", 1, dest_Callback);
    ros::Subscriber odom_sub = n.subscribe("odom", 1, odom_Callback);

    ros::Rate loop_rate(10);

    while(1){
        ros::spinOnce();
        if(working){
          double path_distance = sqrt((targetX - realX)*(targetX - realX)  + (targetY - realY)*(targetY - realY));
          float path_angle = atan2(targetY - realY, targetX - realX);
          float posT = realT;
          float posX = realX;
          float posY = realY;
          geometry_msgs::Twist twist;

          if(state == 0){
            //angle path
            twist.angular.z = angular_speed * (path_angle-posT);
            if(twist.angular.z > 0){
              twist.angular.z = std::min(twist.angular.z, BURGER_MAX_ANG_VEL);
            }else{
              twist.angular.z = std::max(twist.angular.z, -BURGER_MAX_ANG_VEL);
            }
            ROS_INFO("Correction path angle : \n-T %0.2f\n-PA %0.2f", posT, path_angle);

            //forward path
            path_distance = sqrt((targetX - posX)*(targetX - posX)  + (targetY - posY)*(targetY - posY));
            twist.linear.x = std::min(linear_speed*path_distance, BURGER_MAX_LIN_VEL);
            ROS_INFO("Path distance : \n-S %0.2f\n-PD %0.2f", twist.linear.x, path_distance);

            if(initial_path_distance != 0)
              pubPercentage( 1 - path_distance/initial_path_distance);

            if(path_distance <= 0.06){
              state = 2;
              twist.linear.x = 0;
            }

          }else if(state == 2){
            //angle path
            twist.linear.x = 0;
            twist.angular.z = angular_speed * (targetT-posT);
            if(twist.angular.z > 0){
              twist.angular.z = std::min(twist.angular.z, BURGER_MAX_ANG_VEL);
            }else{
              twist.angular.z = std::max(twist.angular.z, -BURGER_MAX_ANG_VEL);
            }
            ROS_INFO("Correction final angle : \n-T %0.2f\n-PA %0.2f", posT, path_angle);

            if(std::abs(targetT-posT) <= 0.01){
              state = 3;
            }
          }else if(state == 3){
            twist.angular.z = 0;
            twist.linear.x = 0;
            state = 4;
            working = false;
          }

          cmd_pub.publish(twist);
        }

        loop_rate.sleep();
  }

    return 0;
}
