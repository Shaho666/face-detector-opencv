#include "VideoFrames.h"



VideoFrames::VideoFrames()
	:m_capture(NULL), m_imgMats(NULL)
{
}

VideoFrames::VideoFrames(String fileName)
{
	m_capture = new VideoCapture(fileName);
	
	m_imgMats = new vector<Mat*>;
	Mat* imgMat = new Mat;
	while (m_capture->read(*imgMat))
	{
		m_imgMats->push_back(imgMat);
		imgMat = new Mat;
	}

	delete imgMat;
}


VideoFrames::~VideoFrames()
{
	if (m_capture != NULL)
	{
		delete m_capture;
		m_capture = NULL;
	}
	for (int i = 0; i < m_imgMats->size(); i++)
	{
		if ((*m_imgMats)[i] != NULL)
		{
			delete (*m_imgMats)[i];
			(*m_imgMats)[i] = NULL;
		}
	}
	if (m_imgMats != NULL)
	{
		delete m_imgMats;
		m_imgMats = NULL;
	}
}

int VideoFrames::GetFrameWidth()
{
	return (int) m_capture->get(CAP_PROP_FRAME_WIDTH);
}

int VideoFrames::GetFrameHeight()
{
	return (int) m_capture->get(CAP_PROP_FRAME_HEIGHT);
}

int VideoFrames::GetFrameFps()
{
	return m_capture->get(CV_CAP_PROP_FPS);
}

int VideoFrames::GetFrameCount()
{
	return m_capture->get(CAP_PROP_FRAME_COUNT);
}

int VideoFrames::GetSize()
{
	if (!m_imgMats)
	{
		return 0;
	}

	return m_imgMats->size();
}

bool VideoFrames::SetPosFrames(int frameToStart)
{
	m_capture->set(CAP_PROP_POS_FRAMES, frameToStart);
	return true;
}

Mat* VideoFrames::GetImgMat(int arrIndex)
{
	if (arrIndex >= m_imgMats->size())
	{
		return NULL;
	}
	return (*m_imgMats)[arrIndex];
}

Mat* VideoFrames::GetImgMat(Mat* target)
{

	for (int i = 0; i < m_imgMats->size(); i++)
	{
		if (m_imgMats->at(i) == target)
		{
			return m_imgMats->at(i);
		}
	}

	return NULL;
}
