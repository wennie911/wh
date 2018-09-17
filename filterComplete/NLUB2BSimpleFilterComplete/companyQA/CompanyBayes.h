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

#define BAYESROWMAX 500 //�ؼ���
#define BAYESCOLMAX 20 //����

typedef struct _struWordData
{
	string sKeyWord;
	float fData[BAYESCOLMAX];
}struWordData;//ÿһ����ѵ�����ݵĽṹ

class CCompanyBayes  
{  
	public:
		CCompanyBayes(string sModule);
		~CCompanyBayes();

		//�õ����ص����
		int getSentenceType(const vector<string> &vecWordsList);
	private:
		map<string, string> m_mapTrainedData;//ѵ���������,�����ݿ�õ�
		float m_originalMatrix[BAYESROWMAX][BAYESCOLMAX]; //ԭ����
		float m_verticalChangeMatrix[BAYESCOLMAX][BAYESROWMAX]; //��ʱ��90��ת��
		CCompanyOnlyQA m_QADB;
		string m_sModule;//Ҫ���õ�ģ��
		
		//��ʼ��
		void init();
		
		//��ȡѵ����Ľṹmap
		bool readTrainedDataToMap(string &sModule);

		//��map���ҵ���Ҫ�Ĵ�
		bool findKeyWordsFromMap(vector<string> vecWordsList,vector<struWordData> &vecWordData);
		bool splitData(float *fDataArr, string sData);
		
		//�Ѵ�ת���ɾ���
		void insertMatrix(vector<struWordData> &vecWordData);
		 //��ʱ��90��ת��
		void changeMatrix(float *fMatrix, float *fMatrixChanged);
		 //�õ�һά��������ֵ
		float getMax(float *fMatrixChanged, int length);
		 //�õ�һ��������ۼӺ�
		float RowDataSum(float *p, int length);


 
};  

#endif
