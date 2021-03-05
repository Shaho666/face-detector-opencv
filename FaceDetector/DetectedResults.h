#ifndef DETECTED_RESULTS
#define DETECTED_RESULTS

#include <vector>
#include <map>
#include <opencv2/highgui.hpp>

using namespace std;
using namespace cv;

class DetectedResults
{
public:
	DetectedResults();
	DetectedResults(DetectedResults* other);
	~DetectedResults();
	bool AddResult(Mat* imgMat, vector<Rect>* faces);
	vector<Rect>* GetResults(Mat* targetKey);
	map<Mat*, vector<Rect>*>* GetTotalResults();
	int GetSize();
private:
	map<Mat*, vector<Rect>*>* m_results;
};

#endif