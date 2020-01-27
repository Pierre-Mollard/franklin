#include <string>
#include <std_msgs/String.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>

ros::Publisher joint_pub;

void ctrl_Callback(const std_msgs::String msg){
    const char * msg_char = msg.data.c_str();
    std::string str(msg_char);
    
    ROS_INFO("callback (%s)", msg_char);
    geometry_msgs::Twist twist;
    
    twist.linear.x = 0;
    twist.linear.y = 0;
    twist.linear.z = 0;
    
    twist.angular.x = 0;
    twist.angular.y = 0;
    twist.angular.z = 0;

    
    if(str.compare("avancer") == 0){
        twist.linear.x = 20;
        joint_pub.publish(twist);
        ROS_INFO("avance OK");
    }else if(str.compare("stop") == 0){
        twist.linear.x = 0;
        twist.linear.y = 0;
        twist.linear.z = 0;
    
        twist.angular.x = 0;
        twist.angular.y = 0;
        twist.angular.z = 0;
        joint_pub.publish(twist);
        ROS_INFO("arret OK");
    }else{
        ROS_INFO("pas compris");
    }
}

int main(int argc, char** argv) {
    
    ros::init(argc, argv, "rouler");
    ros::NodeHandle n;

    joint_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1);
    ros::Subscriber ctrl_sub = n.subscribe("ctrl", 1, ctrl_Callback);
    
    ros::Rate loop_rate(30);
    
    while(1){
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
