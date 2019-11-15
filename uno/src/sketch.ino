#include<PS3BT.h>
#include<PS3USB.h>
#include<usbhub.h>
#include<ros.h>
#include<std_msgs/Bool.h>
#include<Arduino.h>
#include<geometry_msgs/Twist.h>


USB Usb;
BTD Btd(&Usb);
PS3BT PS3(&Btd);

int direction(int x, int y) {
  double deg , rad;
  int pattern = 0;
  x -= 128;
  y = abs(y - 256) - 128;
  rad = atan2((double)y, (double)x);
  deg = rad * 180.0 / (atan(1.0) * 4.0);
  if (abs(x) > 16 || abs(y) > 16) {
    deg += 180.0;
    for (int i = 0; i < 8; i++) {
      if (deg >= 0.0 + (i * 45.0) && deg < 45.0 + (i * 45.0)) {//分割数を指定
        pattern = i + 1;
      }
    }
  }
  return pattern;

}
ros::NodeHandle nh;
geometry_msgs::Twist mg;
std_msgs::Bool msg;

ros::Publisher sw_pub("switch",&msg);
ros::Publisher ord_pub("order",&mg);

void setup(){
	#if !defined(__MIPSEL__)
    	#ifdef DEBUG
    		while (!Serial);
     	#endif

	#endif
    if (Usb.Init() == -1) {
    /*   
      #ifdef DEBUG
      Serial.print(F("\r\nOSC did not start"));
      #endif

      */
      while (1);
    }
	nh.initNode();
	nh.advertise(sw_pub);
	nh.advertise(ord_pub);
}
void loop(){
	Usb.Task();
	if(PS3.PS3Connected||PS3.PS3NavigationConnected){

			int lx = PS3.getAnalogHat(LeftHatX);
    		int ly = PS3.getAnalogHat(LeftHatY);
			int cwx=(50+(lx-128));
			int cwy=(127-(abs(ly-256)))*2;
			int ccwx=abs(-50+(lx-128));
			int ccwy=abs(-128-(abs(ly-256)))*2;

			if (PS3.getAnalogHat(LeftHatX) > 137||PS3.getAnalogHat(LeftHatX) <117 || PS3.getAnalogHat(LeftHatY) > 137 || PS3.getAnalogHat(LeftHatY) < 117){
					switch (direction(PS3.getAnalogHat(LeftHatX), PS3.getAnalogHat(LeftHatY))){
						case 0:
						 	mg.linear.x=0;
							mg.linear.y=0;
							mg.angular.z=0;
							ord_pub.publish(&mg);
							break;
						case 1:
						case 2:
							mg.linear.x=ccwx;
							mg.linear.y=ccwy;
							mg.angular.z=0;
							ord_pub.publish(&mg);
							break;
						case 3:
						case 4:
							mg.linear.x=cwx;
							mg.linear.y=ccwy;
							mg.angular.z=0;
							ord_pub.publish(&mg);
							break;
						case 5:
						case 6:
							mg.linear.x=cwx;
							mg.linear.y=cwy;
							mg.angular.z=0;
							ord_pub.publish(&mg);
							break;
						case 7:
						case 8:
							mg.linear.x=ccwx;
							mg.linear.y=cwy;
							mg.angular.z=0;
							ord_pub.publish(&mg);
							break;
				}
			}
			else{

				if(PS3.getButtonPress(CIRCLE)){
						msg.data=true;
						sw_pub.publish(&msg);
				}

				else if(PS3.getButtonPress(CROSS)){
						msg.data=false;
						sw_pub.publish(&msg);
				}
				else if(PS3.getButtonPress(RIGHT)){
						mg.linear.x=0;
						mg.linear.y=0;
						mg.angular.z=100;
						ord_pub.publish(&mg);
				}
				else if(PS3.getButtonPress(LEFT)){
				    	mg.linear.x=0;
						mg.linear.y=0;
						mg.angular.z=-100;
				}
			}
		}
			nh.spinOnce();
			delay(1);
	}





