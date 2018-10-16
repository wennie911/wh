#ifndef _H_WEBAPI
#define _H_WEBAPI

int ASRWebapiProcessInit(const char *job_UUID, const char *sAudioFile);
void ASRWebapiProcessUninit(const char *job_UUID);
int GetAsrResult(char *pAsrResult);

#endif