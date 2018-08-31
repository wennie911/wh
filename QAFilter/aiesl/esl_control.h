#ifndef _ESL_CONTROL_H
#define _ESL_CONTROL_H

#include <esl.h>

void esl_play_break(esl_handle_t *handle);

void esl_record_start(esl_handle_t *handle, const char *record_file_pathname);

void esl_record_stop(esl_handle_t *handle, const char *record_file_pathname);

void esl_playback(esl_handle_t *handle, const char *filepathname);

void els_bridege(esl_handle_t *handle, const char *call_str);

void esl_play_sound(esl_handle_t *handle, char *sound_path, char *sound_file);

void esl_stop_ASR(esl_handle_t *handle, char *uuid);

#endif
