#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>

class ImageConverter
{
public:
  ImageConverter(std::string input_topic = "/sift_corrections_raw", std::string output_topic = "sift_me_now")
    : it_(nh_), rate_(5), input_topic_(input_topic), output_topic_(output_topic)
  {
    image_pub_ = it_.advertise(output_topic_, 1);
    image_sub_ = it_.subscribe(input_topic_, 1, &ImageConverter::imageCallback, this);
  }
  void imageCallback(const sensor_msgs::ImageConstPtr& msg);
  void publishImage();
  void spin();

private:
  ros::NodeHandle nh_;
  ros::Rate rate_;
  image_transport::ImageTransport it_;
  image_transport::Publisher image_pub_;
  image_transport::Subscriber image_sub_;
  std::string input_topic_;
  std::string output_topic_;
  cv::Mat image_;
};


void ImageConverter::imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  try
  {
    image_ = cv_bridge::toCvShare(msg, "bgr8")->image;
    publishImage();
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
  }
}

void ImageConverter::publishImage()
{
  sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", image_).toImageMsg();
  image_pub_.publish(msg);
}

void ImageConverter::spin()
{
  while (nh_.ok())
  {
    ros::spinOnce();
    rate_.sleep();
  }
}





int main(int argc, char** argv)
{
  std::string node_name = "image_converter" + std::string(argv[1]);
  ros::init(argc, argv, node_name);
  ImageConverter ic(argv[1], argv[2]);
  ic.spin();
  return 0;
}
