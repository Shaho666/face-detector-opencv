#ifndef VIDEO_FRAMES
#define VIDEO_FRAMES

#include <vector>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

class VideoFrames
{
public:
	VideoFrames();
	VideoFrames(String fileName);
	~VideoFrames();
	int GetFrameWidth();
	int GetFrameHeight();
	int GetFrameFps();
	int GetFrameCount();
	int GetSize();
	bool SetPosFrames(int frameToStart);
	Mat* GetImgMat(int arrIndex);
	Mat* GetImgMat(Mat* target);
private:
	VideoCapture* m_capture;
	vector<Mat*>* m_imgMats;
};

#endif
