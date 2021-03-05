#include "DetectedResults.h"



DetectedResults::DetectedResults()
	:m_results(new map<Mat*, vector<Rect>*>)
{
}

DetectedResults::DetectedResults(DetectedResults* other)
{
	if (other == NULL)
	{
		return;
	}

	this->m_results = new map<Mat*, vector<Rect>*>;
	
	map<Mat*, vector<Rect>*>::iterator iter;
	
	map<Mat*, vector<Rect>*>* otherResults = other->GetTotalResults();
	for (iter = otherResults->begin(); iter != otherResults->end(); iter++)
	{
		(*m_results)[iter->first] = new vector<Rect>(*iter->second);
	}

}


DetectedResults::~DetectedResults()
{
	map<Mat*, vector<Rect>*>::iterator iter;
	for (iter = m_results->begin(); iter != m_results->end(); iter++)
	{
		if (iter->second != NULL)
		{
			delete iter->second;
			iter->second = NULL;
		}
	}
	if (m_results != NULL) 
	{
		delete m_results;
		m_results = NULL;
	}
}

bool DetectedResults::AddResult(Mat* imgMat, vector<Rect>* faces)
{

	if (imgMat == NULL || faces == NULL)
	{
		return false;
	}

	if (m_results->count(imgMat) == 0)
	{
		(*m_results)[imgMat] = new vector<Rect>(*faces);
	}
	else
	{
		vector<Rect>* existed = (*m_results)[imgMat];
		existed->insert(existed->end(), faces->begin(), faces->end());
	}
	return true;
}

vector<Rect>* DetectedResults::GetResults(Mat* targetKey)
{

	if (targetKey == NULL)
	{
		return NULL;
	}

	return (*m_results)[targetKey];
}

map<Mat*, vector<Rect>*>* DetectedResults::GetTotalResults()
{
	return m_results;
}

int DetectedResults::GetSize()
{
	if (m_results == NULL)
	{
		return 0;
	}

	return m_results->size();
}
