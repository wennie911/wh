 #ifndef NAIVEBAYES_H_  
#define NAIVEBAYES_H_  
#include<iostream>  
#include<map>  
#include<set>  
#include<cmath>  
#include<vector>  
#include<iostream>
#include<iomanip>
#include<algorithm>  
#include<numeric>  
#include<cstring>  
#include<stdio.h>  
#include<cstdlib>  

#include "CompanyPublic.h"
#include "CompanyOnlyQA.h"


using namespace std;  

#define BAYESROWMAX 500 //关键词
#define BAYESCOLMAX 20 //分类

typedef struct _struWordData
{
	string sKeyWord;
	float fData[BAYESCOLMAX];
}struWordData;//每一个词训练数据的结构

class CCompanyBayes  
{  
	public:
		CCompanyBayes(string sModule);
		~CCompanyBayes();

		//得到返回的类别
		int getSentenceType(const vector<string> &vecWordsList);
	private:
		map<string, string> m_mapTrainedData;//训练后的数据,从数据库得到
		float m_originalMatrix[BAYESROWMAX][BAYESCOLMAX]; //原矩阵
		float m_verticalChangeMatrix[BAYESCOLMAX][BAYESROWMAX]; //逆时针90度转化
		CCompanyOnlyQA m_QADB;
		string m_sModule;//要调用的模块
		
		//初始化
		void init();
		
		//读取训练后的结构map
		bool readTrainedDataToMap(string &sModule);

		//从map中找到需要的词
		bool findKeyWordsFromMap(vector<string> vecWordsList,vector<struWordData> &vecWordData);
		bool splitData(float *fDataArr, string sData);
		
		//把词转化成矩阵
		void insertMatrix(vector<struWordData> &vecWordData);
		 //逆时针90度转化
		void changeMatrix(float *fMatrix, float *fMatrixChanged);
		 //得到一维数组的最大值
		float getMax(float *fMatrixChanged, int length);
		 //得到一个数组的累加和
		float RowDataSum(float *p, int length);


 
};  

#endif
