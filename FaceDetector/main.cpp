//#include <vld.h>
#include <iostream>

#include "VideoFrames.h"
#include "AbstractDetector.h"
#include "DetectedResults.h"
#include "SensesDetector.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv) 
{
	setUseOptimized(true);

	cout << "begin" << endl;
	VideoFrames frames("test1.mp4");
	AbstractDetector detector("D:/OpenCV/opencv/build3.4.5/install/etc/haarcascades/haarcascade_frontalface_alt.xml");

	double scale = 2.0;

	DetectedResults results;
	detector.DetectFace(&frames, scale, false, &results);

	DetectedResults eyeResults;
	SensesDetector eyeDetector("D:/OpenCV/opencv/build3.4.5/install/etc/haarcascades/haarcascade_frontalface_alt.xml", 
		"D:/OpenCV/opencv/build3.4.5/install/etc/haarcascades/haarcascade_eye.xml", &results);
	eyeDetector.DetectSenses(&frames, scale, &eyeResults);

	detector.RenderFrames(&frames, scale, &results);
	eyeDetector.RenderFrames(&frames, scale, &eyeResults);

	VideoWriter writer("out.avi", CV_FOURCC('X', 'V', 'I', 'D'), frames.GetFrameFps(),
		Size(frames.GetFrameWidth(), frames.GetFrameHeight()), true);
	for (int i = 0; i < frames.GetSize(); i++)
	{
		writer.write(*frames.GetImgMat(i));
	}

	cout << "end" << endl;
	return 0;
}
	