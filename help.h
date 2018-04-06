#pragma once
// ////////////////////////////  头文件包含区块 、//////////////////////////////////
#include<opencv2\opencv.hpp>
#include <cv.h> 
#include<iostream> 


using namespace std;
using namespace cv;

////////////////////////////// 全局变量 声明区块 //////////////////////////////////
enum {
	LEFT = 0,   //左图 
	RIGHT = 1,   // 右图 
	TEMP = 2   // 全图
};
enum {
	DETECTION = 0,   //检测  
	CAPTURING = 1,   // 画面截取 
	CALIBRATED = 2   // 校准
};




///////////////////////////// 函数声明区块 、///////////////////////////////////////

 
String intToString(int v);      // 从一幅大图像中，取出一小块图像。 

void help(int number);         //  提供帮助信息
							 
void writePicture(Mat &frame, int type, int num);  //  将图片写入文件 (type)/num.jpg 

void readPicture(Mat &frame, int type, int num); // 读入图片到frame

void saveXYZ(const char*filename, const Mat&mat); // 保存xyz信息到文件 用于深度图

void picture(int num ,string name ,int number ); // 拍摄照片 存放在left right两个文件夹

int goodPictureNumber(); // 计数存放在left right文件夹里的有效照片对的个数
/*
参数num是想要获得的照片的数量 默认20张
照片分别存放在 left right temp三个文件夹里面 每次调用函数数据会更新 
要是想存放在不同的地方请输入name 和自己想要的编号number 但最好不要使用该功能

*/
                      
void calib();  // 获取left right两个文件夹的图片进行标定
                   /*可以获得如下信息
				   M_left M_right D_left D_right 左右相机内参数3*3 畸变参数5*1
				   F 基础矩阵  R 旋转矩阵 T 平移矩阵
				   R1 R2 P1 P2 Q map1_left map2_left map1_right map2_right
				   */
void getDisp(Mat &left, Mat &right, Mat &disp8);   // 获取视差图 并且生成深度图 3D

void getDispInit();  // 测试用的获取深度图的函数
/*
分别读取left right的第一幅图片作为原图片
*/

/////////////////////////////// help() 函数的实现 ///////////////////////////////
void help(int number)
{
	switch (number)
	{
	case -1:
		cout << "帮助提示如下" << endl;
		break;
	case 0:
		cout << " 按下m键，截屏绘制角点   按下ESC键，退出程序" << endl;
		break;
	case 1:
		std::cout << "打开摄像头失败，退出" << endl;
		exit(0);
		break;
	case 2:
		std::cout << "手动退出图片拍摄 按enter继续" << endl;
		break;
	case 3:
		std::cout << "图片写入文件出错 按enter继续" << endl;
		exit(-1);
		break;
	case 4:
		std::cout << "图片读入出错" << endl;
		exit(-1);
		break;
	default:
		break;
	}
	getchar();
}