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
	string sAnswer;
	string sAudio;
}struLayer;

