#ifndef ABSTRACT_DETECTOR
#define ABSTRACT_DETECTOR

#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <omp.h>
#include <time.h>

#include "VideoFrames.h"
#include "DetectedResults.h"

using namespace cv;
using namespace std;

class AbstractDetector
{
public:
	AbstractDetector();
	AbstractDetector(String cascadeFileName);
	virtual ~AbstractDetector();
	bool Pretreat(Mat* img, double scale, Mat* smallImg);
	bool DetectFace(VideoFrames* frames, double scale, bool tryflip, DetectedResults* results);
	virtual bool RenderFrames(VideoFrames* frames, double scale, DetectedResults* detectedResults);
protected:
	String m_cascadeFileName;
};

#endif

