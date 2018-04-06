#pragma once
#include"help.h"

String intToString(int v)      // ��һ����ͼ���У�ȡ��һС��ͼ�� 
{
	char buf[32] = { 0 };
	snprintf(buf, sizeof(buf), "%u", v);

	String str = buf;
	return str;
}
 
void writePicture(Mat &frame, int type, int num)  //  ��ͼƬд���ļ� (type)/num.jpg
{

	string s;
	switch (type)
	{
	case LEFT:
		s = "./left/" + intToString(num) + ".jpg";
		break;
	case RIGHT:
		s = "./right/" + intToString(num) + ".jpg";
		break;
	case TEMP:
		s = "./temp/" + intToString(num) + ".jpg";
		break;
	default:
		help(3);
		break;
	}
	imwrite(s, frame);
}
void readPicture(Mat &frame, int type, int num)// ����ͼƬ��frame
{
	string s;
	switch (type)
	{
	case LEFT:
		s = "./left/" + intToString(num) + ".jpg";
		break;
	case RIGHT:
		s = "./right/" + intToString(num) + ".jpg";
		break;
	case TEMP:
		s = "./temp/" + intToString(num) + ".jpg";
		break;
	default:
		help(3);
		break;
	}
	frame = imread(s);
}
void saveXYZ(const char*filename, const Mat&mat)
{
	const double max_z = 1.0e4;
	FILE* fp = fopen(filename, "wt");
	for (int y = 0; y <mat.rows; y++)
	{
		for (int x = 0; x <mat.cols; x++)
		{
			Vec3f point = mat.at

				<Vec3f>(y, x);
			if (fabs(point[2] - max_z)<FLT_EPSILON || fabs(point[2])>max_z)continue;
			if (point[2]>300)continue;
			fprintf(fp, "%f %f %f \n", point[0], point[1], point[2]);
		}
	}
	fclose(fp);
}
