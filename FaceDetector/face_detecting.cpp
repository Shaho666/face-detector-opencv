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

Config_info config();//����������Ϣ
void Decoding_frame(Config_info);//��Ƶ��ֺ���װ
void Face_detectAndDraw(Mat& img, CascadeClassifier& cascade,
	CascadeClassifier& nestedCascade,
	double scale, bool tryflip);//������ע

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
	//����Ƶ�ļ�����ʵ���ǽ���һ��VideoCapture�ṹ  
	if (!capture.isOpened())
	{
		cout << "open video error";
	}
	/*CV_CAP_PROP_POS_MSEC �C ��Ƶ�ĵ�ǰλ�ã����룩
	CV_CAP_PROP_POS_FRAMES �C ��Ƶ�ĵ�ǰλ�ã�֡��
	CV_CAP_PROP_FRAME_WIDTH �C ��Ƶ���Ŀ��
	CV_CAP_PROP_FRAME_HEIGHT �C ��Ƶ���ĸ߶�
	CV_CAP_PROP_FPS �C ֡���ʣ�֡ / �룩*/
	int frame_width = (int)capture.get(CV_CAP_PROP_FRAME_WIDTH);
	int frame_height = (int)capture.get(CV_CAP_PROP_FRAME_HEIGHT);
	float frame_fps = capture.get(CV_CAP_PROP_FPS);
	int frame_number = capture.get(CV_CAP_PROP_FRAME_COUNT);//��֡��
	int num = 0;//ͳ��֡��  

	VideoWriter writer;
	string video_name = "out.avi";
	int isColor = 1;

	writer = VideoWriter(video_name, CV_FOURCC('X', 'V', 'I', 'D'), frame_fps, Size(frame_width, frame_height), isColor);
	int i = 0;
	Mat img;

	CascadeClassifier cascade, nestedCascade;
	//ѵ���õ��ļ����ƣ������ڿ�ִ���ļ�ͬĿ¼��
	cascade.load(config_info.scascade);
	nestedCascade.load(config_info.snestedcascade);
	//namedWindow("MyVideo", CV_WINDOW_AUTOSIZE);
	vector<Mat> frames;
	for(;i <= frame_number;i++)
	{
		Mat frame;
		//����Ƶ�ж�ȡһ��֡  
		bool bSuccess = capture.read(frame);
		if (!bSuccess)
		{
			cout << "\n�Ѿ����ܴ���Ƶ�ļ���ȡ֡" << endl;
			break;
		}
		//imshow("MyVideo", frame);
		frames.push_back(frame);
		cout << "\r��" << frame_number << "֡��" << "�����" << i+1 << "֡�Ķ�ȡ";
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
			cout << "\r��" << frame_number << "֡��" << "�����" << i << "֡�Ĵ���";
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
	cout << "���й��������" << endl;
}


void Face_detectAndDraw(Mat& img, CascadeClassifier& cascade,
	CascadeClassifier& nestedCascade,
	double scale, bool tryflip)
{
	int i = 0;
	double t = 0;
	//�������ڴ����������������
	vector<Rect> faces, faces2;
	//����һЩ��ɫ��������ʾ��ͬ������
	const static Scalar colors[] = {
		CV_RGB(0,0,255),
		CV_RGB(0,128,255),
		CV_RGB(0,255,255),
		CV_RGB(0,255,0),
		CV_RGB(255,128,0),
		CV_RGB(255,255,0),
		CV_RGB(255,0,0),
		CV_RGB(255,0,255) };
	//������С��ͼƬ���ӿ����ٶ�
	//nt cvRound (double value) ��һ��double�͵��������������룬������һ����������
	Mat gray, smallImg(cvRound(img.rows / scale), cvRound(img.cols / scale), CV_8UC1);
	//ת�ɻҶ�ͼ��Harr�������ڻҶ�ͼ
	cvtColor(img, gray, CV_BGR2GRAY);
	//imshow("�Ҷ�", gray);
	//�ı�ͼ���С��ʹ��˫���Բ�ֵ
	resize(gray, smallImg, smallImg.size(), 0, 0, INTER_LINEAR);
	//imshow("��С�ߴ�", smallImg);
	//�任���ͼ�����ֱ��ͼ��ֵ������
	equalizeHist(smallImg, smallImg);
	//imshow("ֱ��ͼ��ֵ����", smallImg);
	//����ʼ�ͽ�������˺�����ȡʱ�䣬������������㷨ִ��ʱ��
	t = (double)cvGetTickCount();
	//�������
	//detectMultiScale������smallImg��ʾ����Ҫ��������ͼ��ΪsmallImg��faces��ʾ��⵽������Ŀ�����У�1.1��ʾ
	//ÿ��ͼ��ߴ��С�ı���Ϊ1.1��2��ʾÿһ��Ŀ������Ҫ����⵽3�β��������Ŀ��(��Ϊ��Χ�����غͲ�ͬ�Ĵ��ڴ�
	//С�����Լ�⵽����),CV_HAAR_SCALE_IMAGE��ʾ�������ŷ���������⣬��������ͼ��Size(30, 30)ΪĿ���
	//��С���ߴ�
	cascade.detectMultiScale(smallImg, faces,
		1.1, 2, 0
		//|CV_HAAR_FIND_BIGGEST_OBJECT
		//|CV_HAAR_DO_ROUGH_SEARCH
		| CV_HAAR_SCALE_IMAGE
		, Size(30, 30));
	//���ʹ�ܣ���תͼ��������
	if (tryflip)
	{
		flip(smallImg, smallImg, 1);
		//imshow("��תͼ��", smallImg);
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
			//��ʾ����ʱ����С֮ǰ��ͼ���ϱ�ʾ����������������ű��������ȥ
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
		//ͬ�������������
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
	//imshow("ʶ����", img);
	//imwrite(image_name, img);
}


Config_info config()
{
	ifstream in("test.txt");
	string line;
	string s;
	Config_info config_info;
	if (in) // �и��ļ�  
	{
		while (getline(in, line)) // line�в�����ÿ�еĻ��з�  
		{
			istringstream is(line);
			if (line[0] == '\n' || line[0] == '#')//����ע�ͼ�����
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
	else // û�и��ļ�  
	{
		cout << "no such file" << endl;
		config_info.svideo_name = "xxxxxxxxx";
		return config_info;
	}
}