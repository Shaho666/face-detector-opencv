#include<opencv2\opencv.hpp>
#include<string>
#include<iostream>
#include<opencv2\highgui.hpp>
#include<opencv2\core\core.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\objdetect.hpp>
#include <omp.h>

using namespace cv;
using namespace std;

typedef struct Config_info
{
	string svideo_name;
	string scascade;
	string snestedcascade;
}Config_info;

Config_info config();//加载配置信息
void Decoding_frame(Config_info);//视频拆分和组装
void Face_detectAndDraw(Mat& img, CascadeClassifier& cascade,
	CascadeClassifier& nestedCascade,
	double scale, bool tryflip);//人脸标注

int main()
{
	
	Config_info config_info = config();
	cout << config_info.scascade << endl;
	cout << config_info.svideo_name << endl;
	cout << config_info.snestedcascade << endl;
	if (config_info.svideo_name.compare("xxxxxxxx") == 0)
	{
		cout << "error" << endl;
		return 1;
	}
	Decoding_frame(config_info);
	cin.get();
	return 0;
}
void Decoding_frame(Config_info config_info)
{
	cout << "---------------Decoding_frame-----------------" << endl;
	VideoCapture capture(config_info.svideo_name);
	//打开视频文件：其实就是建立一个VideoCapture结构  
	if (!capture.isOpened())
	{
		cout << "open video error";
	}
	/*CV_CAP_PROP_POS_MSEC – 视频的当前位置（毫秒）
	CV_CAP_PROP_POS_FRAMES – 视频的当前位置（帧）
	CV_CAP_PROP_FRAME_WIDTH – 视频流的宽度
	CV_CAP_PROP_FRAME_HEIGHT – 视频流的高度
	CV_CAP_PROP_FPS – 帧速率（帧 / 秒）*/
	int frame_width = (int)capture.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	float frame_fps = capture.get(CV_CAP_PROP_FPS);
	int frame_number = capture.get(CV_CAP_PROP_FRAME_COUNT);//总帧数
	int num = 0;//统计帧数  

	VideoWriter writer;
	string video_name = "out.avi";
	int isColor = 1;

	writer = VideoWriter(video_name, CV_FOURCC('X', 'V', 'I', 'D'), frame_fps, Size(frame_width, frame_height), isColor);
	int i = 0;
	Mat img;

	CascadeClassifier cascade, nestedCascade;
	//训练好的文件名称，放置在可执行文件同目录下
	cascade.load(config_info.scascade);
	nestedCascade.load(config_info.snestedcascade);
	//namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);
	vector<Mat> frames;
	for(;i <= frame_number;i++)
	{
		Mat frame;
		//从视频中读取一个帧  
		bool bSuccess = capture.read(frame);
		if (!bSuccess)
		{
			cout << "\n已经不能从视频文件读取帧" << endl;
			break;
		}
		//imshow("MyVideo", frame);
		frames.push_back(frame);
		cout << "\r共" << frame_number << "帧，" << "已完成" << i+1 << "帧的读取";
	}
	vector<Mat>::iterator f;
	i = 1;
	//#pragma omp parallel for
	{
		for (f = frames.begin();f != frames.end();f++)
		{
			Mat frame;
			frame = *f;
			Face_detectAndDraw(frame, cascade, nestedCascade, 2, 0);
			cout << "\r共" << frame_number << "帧，" << "已完成" << i << "帧的处理";
			i++;
		}
	}
	//f = frames.begin();
	//for (i = 0;i < frame_number;i++)
	cout << "\n---------------Synthetic_videos---------------" << endl;
	for (f = frames.begin();f < frames.end();++f)
	{
		Mat frame;
		frame = *f;
		writer.write(frame);
		//f++;
	}
	capture.release();
	cout << "所有工作已完成" << endl;
}


void Face_detectAndDraw(Mat& img, CascadeClassifier& cascade,
	CascadeClassifier& nestedCascade,
	double scale, bool tryflip)
{
	int i = 0;
	double t = 0;
	//建立用于存放人脸的向量容器
	vector<Rect> faces, faces2;
	//定义一些颜色，用来标示不同的人脸
	const static Scalar colors[] = {
		CV_RGB(0,0,255),
		CV_RGB(0,128,255),
		CV_RGB(0,255,255),
		CV_RGB(0,255,0),
		CV_RGB(255,128,0),
		CV_RGB(255,255,0),
		CV_RGB(255,0,0),
		CV_RGB(255,0,255) };
	//建立缩小的图片，加快检测速度
	//nt cvRound (double value) 对一个double型的数进行四舍五入，并返回一个整型数！
	Mat gray, smallImg(cvRound(img.rows / scale), cvRound(img.cols / scale), CV_8UC1);
	//转成灰度图像，Harr特征基于灰度图
	cvtColor(img, gray, CV_BGR2GRAY);
	//imshow("灰度", gray);
	//改变图像大小，使用双线性差值
	resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
	//imshow("缩小尺寸", smallImg);
	//变换后的图像进行直方图均值化处理
	equalizeHist(smallImg, smallImg);
	//imshow("直方图均值处理", smallImg);
	//程序开始和结束插入此函数获取时间，经过计算求得算法执行时间
	t = (double)cvGetTickCount();
	//检测人脸
	//detectMultiScale函数中smallImg表示的是要检测的输入图像为smallImg，faces表示检测到的人脸目标序列，1.1表示
	//每次图像尺寸减小的比例为1.1，2表示每一个目标至少要被检测到3次才算是真的目标(因为周围的像素和不同的窗口大
	//小都可以检测到人脸),CV_HAAR_SCALE_IMAGE表示不是缩放分类器来检测，而是缩放图像，Size(30, 30)为目标的
	//最小最大尺寸
	cascade.detectMultiScale(smallImg, faces,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		| CV_HAAR_SCALE_IMAGE
		, Size(30, 30));
	//如果使能，翻转图像继续检测
	if (tryflip)
	{
		flip(smallImg, smallImg, 1);
		//imshow("反转图像", smallImg);
		cascade.detectMultiScale(smallImg, faces2,
			1.1, 2, 0
			//|CV_HAAR_FIND_BIGGEST_OBJECT
			//|CV_HAAR_DO_ROUGH_SEARCH
			| CV_HAAR_SCALE_IMAGE
			, Size(30, 30));
		for (vector<Rect>::const_iterator r = faces2.begin(); r != faces2.end(); r++)
		{
			faces.push_back(Rect(smallImg.cols - r->x - r->width, r->y, r->width, r->height));
		}
	}
	t = (double)cvGetTickCount() - t;
	//   qDebug( "detection time = %g ms\n", t/((double)cvGetTickFrequency()*1000.) );
	for (vector<Rect>::const_iterator r = faces.begin(); r != faces.end(); r++, i++)
	{
		Mat smallImgROI;
		vector<Rect> nestedObjects;
		Point center;
		Scalar color = colors[i % 8];
		int radius;

		double aspect_ratio = (double)r->width / r->height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3)
		{
			//标示人脸时在缩小之前的图像上标示，所以这里根据缩放比例换算回去
			center.x = cvRound((r->x + r->width*0.5)*scale);
			center.y = cvRound((r->y + r->height*0.5)*scale);
			radius = cvRound((r->width + r->height)*0.25*scale);
			circle(img, center, radius, color, 3, 8, 0);
		}
		else
			rectangle(img, cvPoint(cvRound(r->x*scale), cvRound(r->y*scale)),
				cvPoint(cvRound((r->x + r->width - 1)*scale), cvRound((r->y + r->height - 1)*scale)),
				color, 3, 8, 0);
		if (nestedCascade.empty())
			continue;
		smallImgROI = smallImg(*r);
		//同样方法检测人眼
		nestedCascade.detectMultiScale(smallImgROI, nestedObjects,
			1.1, 2, 0
			//|CV_HAAR_FIND_BIGGEST_OBJECT
			//|CV_HAAR_DO_ROUGH_SEARCH
			//|CV_HAAR_DO_CANNY_PRUNING
			| CV_HAAR_SCALE_IMAGE
			);
		for (vector<Rect>::const_iterator nr = nestedObjects.begin(); nr != nestedObjects.end(); nr++)
		{
			center.x = cvRound((r->x + nr->x + nr->width*0.5)*scale);
			center.y = cvRound((r->y + nr->y + nr->height*0.5)*scale);
			radius = cvRound((nr->width + nr->height)*0.25*scale);
			circle(img, center, radius, color, 3, 8, 0);
		}
	}
	//imshow("识别结果", img);
	//imwrite(image_name, img);
}


Config_info config()
{
	ifstream in("test.txt");
	string line;
	string s;
	Config_info config_info;
	if (in) // 有该文件  
	{
		while (getline(in, line)) // line中不包括每行的换行符  
		{
			istringstream is(line);
			if (line[0] == '\n' || line[0] == '#')//跳过注释及空行
				continue;
			while (is >> s)
			{
				if (s.compare("video_name") == 0)
					is >> config_info.svideo_name;
				else if (s.compare("cascade") == 0)
					is >> config_info.scascade;
				else if (s.compare("nestedcascade") == 0)
					is >> config_info.snestedcascade;
			}
		}
		return config_info;
		cout << config_info.svideo_name << endl;
		cout << config_info.scascade << endl;
		cout << config_info.snestedcascade << endl;
	}
	else // 没有该文件  
	{
		cout << "no such file" << endl;
		config_info.svideo_name = "xxxxxxxxx";
		return config_info;
	}
}