#ifndef _H_TTS
#define _H_TTS

const char *get_tts_error();
int tts_init();
void tts_uninit();
int text_to_speech(const char* src_text, const char* des_path);
int text_to_speech_sync(const char* src_text, const char* des_path);

#endif