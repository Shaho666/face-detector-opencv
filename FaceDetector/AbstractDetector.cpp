#include "AbstractDetector.h"



AbstractDetector::AbstractDetector()
{
}

AbstractDetector::AbstractDetector(String cascadeFileName)
	:m_cascadeFileName(cascadeFileName)
{
}


AbstractDetector::~AbstractDetector()
{
}

bool AbstractDetector::Pretreat(Mat* img, double scale, Mat* smallImg)
{
	if (img == NULL || smallImg == NULL) 
	{
		return false;
	}
	Mat gray(cvRound(img->rows / scale), cvRound(img->cols / scale), CV_8UC1);
	//ת�ɻҶ�ͼ��Harr�������ڻҶ�ͼ
	cvtColor(*img, gray, CV_BGR2GRAY);
	//�ı�ͼ���С��ʹ��˫���Բ�ֵ
	resize(gray, *smallImg, smallImg->size(), 0, 0, INTER_LINEAR);
	//�任���ͼ�����ֱ��ͼ��ֵ������
	equalizeHist(*smallImg, *smallImg);
	return true;
}

bool AbstractDetector::DetectFace(VideoFrames* frames, double scale, bool tryflip, DetectedResults* results)
{

	if (frames == NULL || scale <= 0 || results == NULL)
	{
		return false;
	}

	double start, stop, durationTime;
	start = time(NULL);

	int cursor = 0;

#pragma omp parallel for //num_threads(8)
	for (int j = 0; j < frames->GetSize(); j++)
	{

		CascadeClassifier cascade(m_cascadeFileName);

		Mat* imgMat = frames->GetImgMat(j);
		if (!imgMat)
		{
			continue;
		}

		Mat smallImg(cvRound(imgMat->rows / scale), cvRound(imgMat->cols / scale), CV_8UC1);
		Pretreat(imgMat, scale, &smallImg);

		vector<Rect> faces;
		cascade.detectMultiScale(smallImg, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
		//���ʹ�ܣ���תͼ��������
		if (tryflip)
		{
			flip(smallImg, smallImg, 1);
			vector<Rect> flipFaces;
			cascade.detectMultiScale(smallImg, flipFaces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
			for (int r = 0; r < flipFaces.size(); r++)
			{
				faces.push_back(Rect(smallImg.cols - flipFaces[r].x - flipFaces[r].width, 
					flipFaces[r].y, flipFaces[r].width, flipFaces[r].height));
			}
		}

		results->AddResult(imgMat, &faces);

#pragma omp critical
		{
			cout << "\rtotal " << frames->GetSize() << " ֡frames, current " << ++cursor << " frame";
		}
		
	}

	stop = time(NULL);
	durationTime = (double) difftime(stop, start);
	cout << "\ncost" << durationTime << " s" << endl;

	return true;
}

bool AbstractDetector::RenderFrames(VideoFrames* frames, double scale, DetectedResults* detectedResults)
{
	if (frames == NULL || scale <= 0 || detectedResults == NULL)
	{
		return false;
	}

	for (int j = 0; j < frames->GetSize(); j++)
	{

		vector<Rect>* faces = detectedResults->GetResults(frames->GetImgMat(j));
		if (!faces)
		{
			continue;
		}

		for (int i = 0; i < faces->size(); i++)
		{
			Point center;
			int radius;

			double aspect_ratio = (double)(*faces)[i].width / (*faces)[i].height;
			if (0.75 < aspect_ratio && aspect_ratio < 1.3)
			{
				//��ʾ����ʱ����С֮ǰ��ͼ���ϱ�ʾ����������������ű��������ȥ
				center.x = cvRound(((*faces)[i].x + (*faces)[i].width * 0.5) * scale);
				center.y = cvRound(((*faces)[i].y + (*faces)[i].height * 0.5) * scale);
				radius = cvRound(((*faces)[i].width + (*faces)[i].height) * 0.25 * scale);
				circle(*frames->GetImgMat(j), center, radius, CV_RGB(255, 255, 255), 3, 8, 0);
			}
			/*else
				rectangle(*videoFrames[j]->GetImgMat(), cvPoint(cvRound((*faces)[i]->x*scale), cvRound(r->y*scale)),
					cvPoint(cvRound((r->x + r->width - 1)*scale), cvRound((r->y + r->height - 1)*scale)),
					color, 3, 8, 0);*/

		}
	}
	return true;
}
