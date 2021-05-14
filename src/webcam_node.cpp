#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

#include <thread>

int main(int argc, char** argv)
{
  ros::init(argc, argv, "webcam_node");
  ros::NodeHandle nh("~");
  image_transport::ImageTransport it(nh);
  
  // Read node parameters
  if(not nh.hasParam("id")) {
    ROS_ERROR("Webcam node: does not have parameter id");
    return -1;
  }
  if(not nh.hasParam("frame_id")) {
    ROS_ERROR("Webcam node: does not have parameter frame_id"); 
    return -1;
  }

  int id=0;
  std::string frame_id;
  nh.getParam("id", id);
  nh.getParam("frame_id", frame_id);

  auto image_pub = it.advertise("/webcam/" + frame_id + "/image", 1);
  // Try to contact the device
  cv::VideoCapture cap(id);
  if(not cap.isOpened()) {
    ROS_ERROR_STREAM("Webcam node: Could not open webcam " << id << " !");
    return -1;
  }
  if(not nh.hasParam("width") or not nh.hasParam("height")) {
    ROS_ERROR_STREAM("Webcam node: Width or height parameters are missing.");
    return -1;
  }
  int width;
  int height;
  nh.getParam("width", width);
  nh.getParam("height", height);
  cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
  cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);

  if(not nh.hasParam("fps")) {
    ROS_ERROR_STREAM("Webcam node: No fps parameter was provided");
    return -1;
  }
  int fps;
  nh.getParam("fps", fps);
  //cap.set(cv::CAP_PROP_FPS, fps);

  std::thread worker([&](){
    // Image message to send
    sensor_msgs::ImagePtr msg;
    // Frame of the webcam
    cv::Mat frame;
    // Image header
    auto header = std_msgs::Header{};
    header.frame_id = frame_id;
    ros::Rate rate(fps);
    // Loop over frames
    while(ros::ok())
    {
      // Grab that frame
      cap >> frame;
      // Set timestamp
      header.stamp = ros::Time::now();
      // Build up the message
      msg = cv_bridge::CvImage(header, "bgr8", frame).toImageMsg();
      // Send the image through it
      image_pub.publish(msg);
      rate.sleep();
    }
  });
  ros::spin();
  worker.join();
  cap.release();
  return 0;
}
