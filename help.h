#pragma once
// ////////////////////////////  ͷ�ļ��������� ��//////////////////////////////////
#include<opencv2\opencv.hpp>
#include <cv.h> 
#include<iostream> 


using namespace std;
using namespace cv;

////////////////////////////// ȫ�ֱ��� �������� //////////////////////////////////
enum {
	LEFT = 0,   //��ͼ 
	RIGHT = 1,   // ��ͼ 
	TEMP = 2   // ȫͼ
};
enum {
	DETECTION = 0,   //���  
	CAPTURING = 1,   // �����ȡ 
	CALIBRATED = 2   // У׼
};




///////////////////////////// ������������ ��///////////////////////////////////////

 
String intToString(int v);      // ��һ����ͼ���У�ȡ��һС��ͼ�� 

void help(int number);         //  �ṩ������Ϣ
							 
void writePicture(Mat &frame, int type, int num);  //  ��ͼƬд���ļ� (type)/num.jpg 

void readPicture(Mat &frame, int type, int num); // ����ͼƬ��frame

void saveXYZ(const char*filename, const Mat&mat); // ����xyz��Ϣ���ļ� �������ͼ

void picture(int num ,string name ,int number ); // ������Ƭ �����left right�����ļ���

int goodPictureNumber(); // ���������left right�ļ��������Ч��Ƭ�Եĸ���
/*
����num����Ҫ��õ���Ƭ������ Ĭ��20��
��Ƭ�ֱ����� left right temp�����ļ������� ÿ�ε��ú������ݻ���� 
Ҫ�������ڲ�ͬ�ĵط�������name ���Լ���Ҫ�ı��number ����ò�Ҫʹ�øù���

*/
                      
void calib();  // ��ȡleft right�����ļ��е�ͼƬ���б궨
                   /*���Ի��������Ϣ
				   M_left M_right D_left D_right ��������ڲ���3*3 �������5*1
				   F ��������  R ��ת���� T ƽ�ƾ���
				   R1 R2 P1 P2 Q map1_left map2_left map1_right map2_right
				   */
void getDisp(Mat &left, Mat &right, Mat &disp8);   // ��ȡ�Ӳ�ͼ �����������ͼ 3D

void getDispInit();  // �����õĻ�ȡ���ͼ�ĺ���
/*
�ֱ��ȡleft right�ĵ�һ��ͼƬ��ΪԭͼƬ
*/

/////////////////////////////// help() ������ʵ�� ///////////////////////////////
void help(int number)
{
	switch (number)
	{
	case -1:
		cout << "������ʾ����" << endl;
		break;
	case 0:
		cout << " ����m�����������ƽǵ�   ����ESC�����˳�����" << endl;
		break;
	case 1:
		std::cout << "������ͷʧ�ܣ��˳�" << endl;
		exit(0);
		break;
	case 2:
		std::cout << "�ֶ��˳�ͼƬ���� ��enter����" << endl;
		break;
	case 3:
		std::cout << "ͼƬд���ļ����� ��enter����" << endl;
		exit(-1);
		break;
	case 4:
		std::cout << "ͼƬ�������" << endl;
		exit(-1);
		break;
	default:
		break;
	}
	getchar();
}