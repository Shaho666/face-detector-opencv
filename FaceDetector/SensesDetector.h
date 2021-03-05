#ifndef SENSES_DETECTOR
#define SENSES_DETECTOR

#include <omp.h>

#include "AbstractDetector.h"
#include "DetectedResults.h"

using namespace cv;

class SensesDetector :
	public AbstractDetector
{
public:
	SensesDetector();
	SensesDetector(String cascadeFileName, String sensesFileName);
	SensesDetector(String cascadeFileName, String sensesFileName, DetectedResults* detectedFaces);
	~SensesDetector();
	bool SetCachedFaces(DetectedResults* detectedFaces);
	bool DetectSenses(VideoFrames* frames, double scale, DetectedResults* results);
	bool RenderFrames(VideoFrames* frames, double scale, DetectedResults* detectedResults);
private:
	DetectedResults* m_cachedFaces;
	String m_sensesFileName;
};

#endif