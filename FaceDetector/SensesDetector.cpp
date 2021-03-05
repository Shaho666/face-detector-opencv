#include "SensesDetector.h"



SensesDetector::SensesDetector()
	:m_cachedFaces(NULL)
{
}

SensesDetector::SensesDetector(String cascadeFileName, String sensesFileName)
	: m_cachedFaces(NULL)
{
	this->m_cascadeFileName = cascadeFileName;
	this->m_sensesFileName = sensesFileName;
}

SensesDetector::SensesDetector(String cascadeFileName, String sensesFileName, DetectedResults* detectedFaces)
{
	this->m_cascadeFileName = cascadeFileName;
	this->m_sensesFileName = sensesFileName;
	this->m_cachedFaces = new DetectedResults(detectedFaces);
}


SensesDetector::~SensesDetector()
{
	if (m_cachedFaces != NULL)
	{
		delete m_cachedFaces;
		m_cachedFaces = NULL;
	}
}

bool SensesDetector::SetCachedFaces(DetectedResults* detectedFaces)
{

	if (detectedFaces == NULL)
	{
		return false;
	}

	if (m_cachedFaces != NULL)
	{
		delete m_cachedFaces;
		m_cachedFaces = NULL;
	}

	this->m_cachedFaces = new DetectedResults(detectedFaces);
	return true;
}

bool SensesDetector::DetectSenses(VideoFrames* frames, double scale, DetectedResults* results)
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

		CascadeClassifier cascade(m_sensesFileName);

		Mat* imgMat = frames->GetImgMat(j);
		if (!imgMat)
		{
			continue;
		}

		Mat smallImg(cvRound(imgMat->rows / scale), cvRound(imgMat->cols / scale), CV_8UC1);
		Pretreat(imgMat, scale, &smallImg);

		
		vector<Rect>* cachedFaces = m_cachedFaces->GetResults(imgMat);
		for (int k = 0; k < cachedFaces->size(); k++)
		{
			vector<Rect> senses;
			Mat smallImgROI = smallImg((*cachedFaces)[k]);
			cascade.detectMultiScale(smallImgROI, senses, 1.1, 2, 0 | CASCADE_SCALE_IMAGE);
			//������λ�ý�һ������
			for (int iter = 0; iter < senses.size(); iter++)
			{
				senses[iter].x += (*cachedFaces)[k].x;
				senses[iter].y += (*cachedFaces)[k].y;
			}
			results->AddResult(imgMat, &senses);
		}

#pragma omp critical
		{
			cout << "\rtotal" << frames->GetSize() << "֡frames" << ", current" << ++cursor << "frame";
		}

	}

	stop = time(NULL);
	durationTime = (double) difftime(stop, start);
	cout << "\ncost" << durationTime << " s" << endl;

	return true;
}

bool SensesDetector::RenderFrames(VideoFrames* frames, double scale, DetectedResults* detectedResults)
{

	if (frames == NULL || scale <= 0 || detectedResults == NULL)
	{
		return false;
	}

	for (int j = 0; j < frames->GetSize(); j++)
	{
		vector<Rect>* senses = detectedResults->GetResults(frames->GetImgMat(j));
		if (!senses)
		{
			continue;
		}

		for (int i = 0; i < senses->size(); i++)
		{
			Point center;
			int radius;

			center.x = cvRound(((*senses)[i].x + (*senses)[i].width * 0.5) * scale);
			center.y = cvRound(((*senses)[i].y + (*senses)[i].height * 0.5) * scale);
			radius = cvRound(((*senses)[i].width + (*senses)[i].height) * 0.25 * scale);
			circle(*frames->GetImgMat(j), center, radius, CV_RGB(192, 0, 0), 3, 8, 0);
		}
	}

	return true;
}
