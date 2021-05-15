# Webcam

`Webcam` is a simple ROS node that publish images of a camera. It supports the compressed image transport layer from ROS.
The launch file provides an easy way to configure the index of the camera you want to use, the `height` and `width` of the image, and also the desired `frame rate`. The `frame_id` is also mandatory.
