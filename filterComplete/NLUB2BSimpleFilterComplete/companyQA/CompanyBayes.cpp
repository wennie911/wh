#include "CompanyBayes.h" 
#include "../AICC/KGPublic.h"


CCompanyBayes::CCompanyBayes(string sModule)
{		
		m_sModule = sModule;
		init();
}
CCompanyBayes::~CCompanyBayes()
{
	m_sModule="";
}

void CCompanyBayes::init()
{
	m_mapTrainedData.clear();

	readTrainedDataToMap(m_sModule);
}

int CCompanyBayes::getSentenceType(const vector<string> &vecWordsList)
{
	//如果词集合为空，不做查找返回0
	if(m_mapTrainedData.empty())
		return 0;
	
	memset(m_originalMatrix, 0, sizeof(m_originalMatrix));
	memset(m_verticalChangeMatrix, 0, sizeof(m_verticalChangeMatrix));
	

	if(vecWordsList.empty())
		return 0;
	//是否有词匹配
	vector<struWordData> vecWordData;
	if(!findKeyWordsFromMap(vecWordsList, vecWordData))
		return 0;

	//把数据移到矩阵中
	for(int i=0; i<vecWordData.size(); i++)
	{
		memcpy(&m_originalMatrix[i][0], &(vecWordData.at(i).fData[0]), BAYESCOLMAX);
	}

	//把矩阵逆时针转动90度
	changeMatrix(&m_originalMatrix[0][0], &m_verticalChangeMatrix[0][0]);

	//计算每一行的累加和
	float fSumArr[BAYESROWMAX]={0};

	for(int i=0; i<BAYESCOLMAX; i++)
	{
		if(m_verticalChangeMatrix[i][0]==0.0)
			break;
		fSumArr[i] = RowDataSum(&m_verticalChangeMatrix[i][0], BAYESROWMAX);
	}

	float fMax = getMax(fSumArr, BAYESCOLMAX);

	int iMatchType=0;
	for(; iMatchType < BAYESCOLMAX; iMatchType++)
	{
		if(fSumArr[iMatchType] == 0)
			break;
		if(fSumArr[iMatchType] == fMax)
			break;
	}

	return iMatchType+1;
}


bool CCompanyBayes::readTrainedDataToMap(string &sModule)
{
	return m_QADB.getBayesTrainedData(m_mapTrainedData, sModule);
}

bool CCompanyBayes::findKeyWordsFromMap(vector<string> vecWordsList, vector<struWordData> &vecWordData)
{
	map<string, string>::iterator iterMap;
	for(vector<string>::iterator iter=vecWordsList.begin(); iter != vecWordsList.end(); iter++)
	{
		iterMap = m_mapTrainedData.find(*iter);
		if(iterMap == m_mapTrainedData.end())
		{
			continue;
		}

		//找到了，把词取出来, 并处理
		struWordData swd;
		memset(&swd.fData, 0, BAYESCOLMAX);
		swd.sKeyWord = iterMap->first;
		string sData = iterMap->second;
		if(!splitData(&swd.fData[0], sData))
		{
			continue;
		}
		vecWordData.push_back(swd);
		
	}
	return !vecWordData.empty();
}

bool CCompanyBayes::splitData(float *fDataArr, string sData)
{
	CKGPublic kp;
	vector<string> vecData;
	vector<float> vecFData;
	
	if(kp.kg_split(sData, vecData, "|")==0)
		return false;

	for(vector<string>::iterator iter=vecData.begin(); iter!=vecData.end(); iter++)
	{
		float fD = atof(iter->c_str());
		vecFData.push_back(fD);
	}

	//会溢出吗？
	for(int i=0; i<vecFData.size(); i++)
	{
		*(fDataArr+i)=vecFData.at(i);
	}

	return !vecFData.empty();
}


//矩阵转置,将原始矩阵将行列转置
void CCompanyBayes::changeMatrix(float *fMatrix, float *fMatrixChanged)
{
	*fMatrixChanged=*fMatrix;
	for(int i=0; i<BAYESCOLMAX; i++)
	{
		for(int j=0; j<BAYESROWMAX; j++)
		{
			if(*(fMatrix+i+j*BAYESCOLMAX)==0)
				break;
			*(fMatrixChanged+j+i*BAYESROWMAX) = *(fMatrix+i+j*BAYESCOLMAX);
		}
	}
}

float CCompanyBayes::RowDataSum(float *p, int length)
{
	float pSum = 0.0;
	for(int i=0; i<length; i++)
	{
		if(*(p+i)==0.0)
		  break; 
		pSum += *(p+i);
	}
	return pSum;
}
//寻找最大值(绝对值最大)
float CCompanyBayes::getMax(float *p, int length)
{
	float fMax = -100.0;
	for(int i=0; i<length; i++)
	{ 
		if(*(p+i)==0.0)
			break; 
		(*(p+i)> fMax) ? fMax=*(p+i) :fMax=fMax;
		
	}
	return fMax;
}


