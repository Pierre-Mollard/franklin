#include <string>
#include <std_msgs/String.h>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>


void ctrl_Callback(const std_msgs::String msg){
    const char * msg_char = msg.data.c_str();
    std::string str(msg_char);

    ROS_INFO("callback (%s)", msg_char);

}

int main(int argc, char** argv) {

    ros::init(argc, argv, "frkl_ctrl");
    ros::NodeHandle n;

    ros::Subscriber ctrl_sub = n.subscribe("ctrl", 1, ctrl_Callback);

    ros::Rate loop_rate(30);

    while(1){
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
