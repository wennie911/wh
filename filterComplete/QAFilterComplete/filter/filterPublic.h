#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <vector>
#include <string>

#define BUFSTRINGLEN 1024*10

using namespace std;

typedef struct _struNLUQA
{
	string sQuestion;
	string sAnswer;
	string sMatchedQuestion;
	string sAudio;
	string sType;
	string sQuestionPinYin;
	string sQuestionType;
	int iIsEnd;
	int iIsBreak;
	int iLoop;
	int iAffirm;
	int iIsDel;
}struNLUQA;

typedef struct _struLayer
{
	int iLayerId;
	int iInitiative;
	int iUsed;
	vector<string> vecLeakList;
	string sDefQuestion;
	string sDefDenyQuestion;
	string sDefWhenQuestion;
	string sDefWhereQuestion;
	string sDefWhoQuestion;
	string sDefHowQuestion;
	string sDefWhatQuestion;
	string sDefWhichQuestion;
	string sDefWhyQuestion;
	string sAnswer;
	string sAudio;
}struLayer;

