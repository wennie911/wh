#ifndef _AI_H
#define _AI_H
#include "nluProcess.h"
void set_ai_loglevel(int loglevel);

int ai_initialize(const char *module_pathname, const char *work_dir);

void ai_uninit();

const char *ai_get_info();


void dojsonlist(const char *text, char *words);
int ai_get_answer(char * sAnswer, struRetAnswer *retAnswerPtr);

//读取关键字文件
int getKeyWordsFile(char *sFilename);
#endif

