#ifndef _H_LOAD_CONFIG
#define _H_LOAD_CONFIG

int GetProfileString(char *profile, char *AppName, char *KeyName, char *KeyVal);
int GetProfileInt(char *profile, char *AppName, char *KeyName, int *KeyVal);

#endif