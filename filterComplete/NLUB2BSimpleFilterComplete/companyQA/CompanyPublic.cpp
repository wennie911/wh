#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h> 

#include "CompanyPublic.h"
#include "CompanyQAOperateBase.h"

#include "../json/json_writer.h"
#include "../commonlib/base64.h"
#include "../datalibs_management/CharacterDB.h"


CCompanyPublic::CCompanyPublic()
{
	initCommandString();
}

CCompanyPublic::~CCompanyPublic()
{
}

bool CCompanyPublic::initCommandString()
{
	CCompanyQAOperateBase qadb;
	qadb.getCommand(m_mapCommandCn);
	qadb.getNormal(m_mapNormalCn);
	
	return true;
}


string CCompanyPublic::getNewString(const string sEn, int iType)
{
	switch(iType)
	{
	case COMPANY_COMMAND:
		{
			map<string, string >::iterator iter = m_mapCommandCn.find(sEn);
			if(iter == m_mapCommandCn.end())
				return " ";
			return iter->second;
		}
	case COMPANY_NORMAL:
		{
			map<string, string >::iterator iter = m_mapNormalCn.find(sEn);
			if(iter == m_mapNormalCn.end())
				return " ";
			return iter->second;
		}
	
	default:
		return " ";
	}
	return " ";
}

string CCompanyPublic::formatNewString(const string originalStr,const string insertStr)
{
	string st = getNewString(originalStr,COMPANY_NORMAL);
	int pos = st.find("%");
	if(pos != string::npos)
	{
		st.erase(pos,1);
		st.insert(pos,insertStr);
	}
	
	return st;
}

string CCompanyPublic::formatNewString(const string originalStr, int iParamCount, const char *str, ...)
{
	string st = getNewString(originalStr,COMPANY_NORMAL);
	//PrintI("formatNewString st=%s", st.c_str());
	string stParam="";
	va_list arg_ptr;
	va_start(arg_ptr, str);

	stParam = str;
	int pos = st.find("%");
	if(pos != string::npos)
	{
		st.erase(pos,1);
		st.insert(pos,stParam);
	}
	else
		return st;

	for(int i=0; i < iParamCount-1; i++)
	{
		stParam.assign(va_arg(arg_ptr, char *));
		int pos = st.find("%");
		if(pos != string::npos)
		{
			st.erase(pos,1);
			st.insert(pos,stParam);
		}
		else
			break;
	
	}
	va_end(arg_ptr);
	return st;
}

//替换新字符串,用于接口
string CCompanyPublic::formatNewStringUseOfInterface(const string originalStr, int iParamCount, const char *str, ...)
{
	string st = originalStr;
	//PrintI("formatNewString st=%s", st.c_str());
	string stParam="";
	va_list arg_ptr;
	va_start(arg_ptr, str);

	stParam = str;
	int pos = st.find("%");
	if(pos != string::npos)
	{
		st.erase(pos,1);
		st.insert(pos,stParam);
	}
	else
		return st;

	for(int i=0; i < iParamCount-1; i++)
	{
		stParam.assign(va_arg(arg_ptr, char *));
		int pos = st.find("%");
		if(pos != string::npos)
		{
			st.erase(pos,1);
			st.insert(pos,stParam);
		}
		else
			break;
	
	}
	va_end(arg_ptr);
	return st;
}



/*
{
"text":"",                         //文本输出，TTS使用。唯一答案情况使用
"mult-q":["","","",""],    //四宫格，备选问题，2~4个，此时忽略"text"字段
"image":"url",                 //图片文件，只支持url
"audio":"url",                  //音频文件，只支持url
"vedio":"url",                 //视频文件，只支持url
"action":"actn_wave"    //动作，命令列表后面将重新设计
"param":["菜名","份数","辣度","口味","忌口"]
}

*/
//用于四宫格
//sImage 作为answerType使用
string CCompanyPublic::makeSenenceJson(vector<struFullQA> &vecQA)
{
	
	Json::Value arrayQA;
	Json::FastWriter writer;
	string sCounter="0";
	string sQuestionPinYin="";
	
	if(vecQA.empty())
	{
		
			return "";
				
	}
	for(vector<struFullQA>::iterator iter = vecQA.begin(); iter != vecQA.end(); iter++)
	{
		Json::Value jQA;
	    jQA["question"] = Json::Value(iter->sQuestion);
		jQA["answer"] = Json::Value(iter->sAnswer);
		jQA["type"] = Json::Value(iter->media.sType);
		jQA["matchedQuestion"] = Json::Value(iter->sMatchedQuestion);
		jQA["audio"] = Json::Value(iter->media.sAudio);
		jQA["isEnd"] = Json::Value(iter->media.sIsEnd);
	    jQA["isBreak"] = Json::Value(iter->media.sIsBreak);
		jQA["loop"] = Json::Value(iter->media.sLoop);
		jQA["affirm"] = Json::Value(iter->media.sAffirm);
		jQA["questionType"] = Json::Value(iter->media.sQuestionType);
		jQA["questionPinYin"] = Json::Value(getPinYin(iter->sQuestion));
		arrayQA.append(jQA);
	}
	
	PrintI("%s",writer.write(arrayQA).c_str());
	return writer.write(arrayQA);
}

string CCompanyPublic::getPinYin(string sSentence)
{
	string sPinYin="";
	if(sSentence.empty())
		return "";
	vector<word_note> notes;
	CCharacterDB::GetInstance()->GetStringPinyin(sSentence, notes);
	for(vector<word_note>::iterator iter=notes.begin(); iter!=notes.end(); iter++)
	{
		sPinYin.append(iter->py);
		sPinYin.append("|");
	}
	return sPinYin;
	
}

string CCompanyPublic::makeBase64(string sJson)
{
	string sJson64= CBase64::Encode((const unsigned char *)sJson.c_str(), sJson.size());
	sJson64 = trimRN(sJson64);
	sJson64.insert(0, "@#@");
	return sJson64;
}

string CCompanyPublic::trimRN(string sInputBase64Str)
{
	string sRetStr;
	for(int i=0; i<sInputBase64Str.size(); i++)
	{
		if(sInputBase64Str.at(i)=='\r'|| sInputBase64Str.at(i)=='\n')
		{
			continue;
		}
		sRetStr.append(1, sInputBase64Str.at(i));
	}
	return sRetStr;
}



string CCompanyPublic::makeLog(string sInput, string sAnswer, vector<struQuestionList> vecSentenceList, string sContentype, int iType, int iSelectIndex)
{
		
	Json::Value root;
	Json::FastWriter writer;

	root["F_INPUT"] = Json::Value(sInput);
	root["F_ANSWER"] = Json::Value(sAnswer);
	root["F_CONTENTTYPE"] = Json::Value(sContentype);
	root["LOG_TYPE"] = Json::Value("STAT");
	root["F_INDEX"] = Json::Value(iSelectIndex);

	if(iType==1)
	{
		for(vector<struQuestionList>::iterator iter=vecSentenceList.begin(); iter!=vecSentenceList.end(); iter++)
		{
			
			root["F_COMMEND_ANSWER"].append(iter->sSentence);

		}
	}
	else
	{
		root["F_COMMEND_ANSWER"] = Json::Value(Json::arrayValue);
	}
	
	if(sAnswer.empty())
	{
		root["F_TYPE"] = Json::Value(2);
	}
	else
	{
		root["F_TYPE"] = Json::Value(iType);
	}
	return writer.write(root);
}

//匹配关键词
bool CCompanyPublic::matchPinYin(string &sSentence, vector<struKeyWord> &vecKeyWords, string &sMatchWord)
{
	//将sSentence 转成拼音
	vector<string> vecInputPY;
	string sApproximatelyPinYin;
	string sFirstPY;
	
	vector<word_note> notes;
	CCharacterDB::GetInstance()->GetStringPinyin(sSentence, notes);

	for(vector<word_note>::iterator iter=notes.begin(); iter!=notes.end(); iter++)
	{
		string sPy = getApproximatelyPinYin(iter->py);
		if(sPy.empty())
			continue;
		vecInputPY.push_back(sPy);
		sFirstPY.append(1, sPy.at(0));
		sApproximatelyPinYin.append(sPy);
	}

	map<string, int> mapNameMatched;
	
	//将预匹配拼音作为一个字符串进行匹配
	for(vector<struKeyWord>::iterator iter = vecKeyWords.begin(); iter!= vecKeyWords.end(); iter++)
	{
		if(iter->sPinYin.empty())
			continue;
		if(sApproximatelyPinYin.find(iter->sPinYin)!=string::npos)
		{
			mapNameMatched[iter->sWord]=iter->sWord.size();
		}
	}

	if(mapNameMatched.size()>0)
	{
		//找到一个匹配度最大的
		int iMax=0;
		for(map<string, int>::iterator iter=mapNameMatched.begin(); iter !=mapNameMatched.end(); iter++)
		{
			if(iMax <= iter->second)
			{
				iMax=iter->second;
				sMatchWord = iter->first;
			}
		}
		PrintI("step 1 matchWord=%s", sMatchWord.c_str());
		return true;
	}

	//用首字母(声母)进行匹配
	map<string, int> mapNameDistance;
	
	for(vector<struKeyWord>::iterator iter = vecKeyWords.begin(); iter!= vecKeyWords.end(); iter++)
	{
		if(iter->sFirstPY.empty())
			continue;
		if(iter->sFirstPY.size()<=2)
			continue;

		int iRet = sFirstPY.find(iter->sFirstPY);
		if(iRet != string::npos)
		{
			//将原拼音串中匹配的拼音串取出来
			vector<string> vecSubPY;
			for(int i=iRet; i<(iRet+iter->sFirstPY.size()); i++)
			{
				vecSubPY.push_back(vecInputPY.at(i));
				PrintI("vecInputPY.at(%d)=%s", i, vecInputPY.at(i).c_str());
			}
			
			int iDistance = getPYDistance(vecSubPY, iter->vecLastPY);
			mapNameDistance[iter->sWord]=iDistance;

		}
			
	}
	
	//如果map大于0, 则选一个距离最小的输出
	if(mapNameDistance.size()>0)
	{
		int iMix=50;
		for(map<string, int>::iterator iter=mapNameDistance.begin(); iter !=mapNameDistance.end(); iter++)
		{
			if(iMix >= iter->second)
			{
				iMix = iter->second;
				sMatchWord = iter->first;
			}
		}
		PrintI("step 2 matchWord=%s", sMatchWord.c_str());
		return true;

	}
	
	
	//当关键字大于3时，用首字母逐一匹配，当有60%匹配时，可以认为匹配成功
	for(vector<struKeyWord>::iterator iter = vecKeyWords.begin(); iter!= vecKeyWords.end(); iter++)
	{
		if(iter->vecFirstPY.size()<=3)
			continue;
		
		if(matchingRateForPinYin(sFirstPY, iter->sFirstPY)>=0.7)
		{
			sMatchWord = iter->sWord;
			PrintI("step 3 matchWord=%s", sMatchWord.c_str());
			return true;
		}
	}

	//混合模式匹配
	for(vector<struKeyWord>::iterator iter = vecKeyWords.begin(); iter!= vecKeyWords.end(); iter++)
	{
		if(iter->vecFirstPY.size()<=4)
			continue;
		for(vector<string>::iterator iterInPY = vecInputPY.begin(); iterInPY != vecInputPY.end(); iterInPY++)
		{
			if(iterInPY->find(iter->vecMixture.at(0))!=string::npos)
			{
				vector<string>::iterator iterInPY1 = iterInPY;
				for(vector<string>::iterator iterMixture = iter->vecMixture.begin(); iterMixture != iter->vecMixture.end() || iterInPY1 != vecInputPY.end(); iterMixture++, iterInPY1++)
				{
					if(iterInPY1->find(*iterMixture)!=string::npos)
						return false;
				}
				sMatchWord = iter->sWord;
				PrintI("step 4 matchWord=%s", sMatchWord.c_str());
				return true;
			}
		}
	}

	return false;
}


//转成模糊音
string CCompanyPublic::getApproximatelyPinYin(string sSrcPY)
{
	
	string strATmp;
	strATmp=sSrcPY;

	strATmp=StringReplace(strATmp,"l","n");
	strATmp=StringReplace(strATmp,"f","h");
	strATmp=StringReplace(strATmp,"sh","s");
	strATmp=StringReplace(strATmp,"ch","c");
	strATmp=StringReplace(strATmp,"zh","z");
	strATmp=StringReplace(strATmp,"ou","o");
	strATmp=StringReplace(strATmp,"ui","ei");
	strATmp=StringReplace(strATmp,"eng","en");
	strATmp=StringReplace(strATmp,"ing","in");
	strATmp=StringReplace(strATmp,"ang","an");
	strATmp=StringReplace(strATmp,"eng","en");
	
	

	return strATmp;
}

int CCompanyPublic::getPYDistance(vector<string> &vecSrc, vector<string> &vecDes)
{
	int j=0;
	int iDistance=0;

	if(vecSrc.size()!= vecDes.size())
		return 10;
	//由于声母相同，判定韵母距离即可
	for(int i=0; i < vecSrc.size(); i++)
	{
		if(vecSrc.at(i).size()<=1)
			continue;
		string sLast = vecSrc.at(i).substr(1);
		
		if(sLast.compare(vecDes.at(i))!=0)
		{
			iDistance++;
		}
	}
	
	return iDistance;
}

//处理输入的拼音和待匹配的拼音不完全相同(漏字情况)
float CCompanyPublic::matchingRateForPinYin(string sInFirstPY, string sFirstPY)
{
	float fCheckedNum=0;
	float fFirstPY=sFirstPY.size();
	for(int i=0, k=0; i<sInFirstPY.size(); i++, k++)
	{
		for(int j=k; j<sFirstPY.size(); j++)
		{
			if(sInFirstPY.at(i)==sFirstPY.at(j))
			{
				sInFirstPY.erase(0,i);
				sFirstPY.erase(0,j);
				fCheckedNum++;
				break;
			}
		}
	}

	return fCheckedNum/fFirstPY;
	
}

