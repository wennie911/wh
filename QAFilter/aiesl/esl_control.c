#include "esl_control.h"

#include <esl.h>
#include <stdio.h>
#include <stdlib.h>

void esl_play_break(esl_handle_t *handle)
{
	esl_log(ESL_LOG_INFO, "esl_break_play start\n");

	char break_cmd[256];

	sprintf(break_cmd, "api uuid_break %s all", esl_event_get_header(handle->info_event, "Channel-Unique-Id"));

	esl_log(ESL_LOG_INFO, "esl_break_play cmd:%s!\n", break_cmd);

	esl_send(handle, break_cmd);

	esl_log(ESL_LOG_INFO, "esl_break_play end!\n");
}

void esl_record_start(esl_handle_t *handle, const char *record_file_pathname)
{
	esl_log(ESL_LOG_INFO, "esl_record_start start\n");

	char record_cmd[256];
	/*
	sprintf(record_cmd, "api uuid_setvar %s RECORD_STEREO=true", esl_event_get_header(handle->info_event, "Channel-Unique-Id"), record_file_pathname);
	esl_log(ESL_LOG_INFO, "esl_record_start cmd:%s!\n", record_cmd);
	esl_send(handle, record_cmd);
	*/

	sprintf(record_cmd, "api uuid_record %s start %s", esl_event_get_header(handle->info_event, "Channel-Unique-Id"), record_file_pathname);
	esl_log(ESL_LOG_INFO, "esl_record_start cmd:%s!\n", record_cmd);
	esl_send(handle, record_cmd);

	esl_log(ESL_LOG_INFO, "esl_record_start end!\n");
}

 void esl_record_stop(esl_handle_t *handle, const char *record_file_pathname)
{
	esl_log(ESL_LOG_INFO, "esl_record_stop start\n");

	char record_cmd[256];

	sprintf(record_cmd, "api uuid_record %s stop %s ", esl_event_get_header(handle->info_event, "Channel-Unique-Id"), record_file_pathname);

	esl_log(ESL_LOG_INFO, "esl_record_stop cmd:%s!\n", record_cmd);

	esl_send(handle, record_cmd);

	esl_log(ESL_LOG_INFO, "esl_record_stop end!\n");
}


void esl_playback(esl_handle_t *handle, const char *filepathname)
{
	//playback_terminators=none

	esl_log(ESL_LOG_INFO, "esl_playback:%s start!\n", filepathname);

	esl_execute(handle, "playback", filepathname, NULL);

	esl_log(ESL_LOG_INFO, "esl_playback:%s end!\n", filepathname);
}

void els_bridege(esl_handle_t *handle, const char *call_str)
{
	esl_execute(handle, "bridge", "{call_timeout=10}sofia/gateway/aio100/13770760596", NULL);
}

void esl_stop_ASR(esl_handle_t *handle, char *uuid)
{
	esl_log(ESL_LOG_INFO, "esl_stop_ASR start\n");

	esl_execute(handle,  "stop_asr", "", NULL);

	esl_log(ESL_LOG_INFO, "esl_stop_ASR end!\n");
}

void esl_play_sound(esl_handle_t *handle, char *sound_path, char *sound_file)
{
	esl_log(ESL_LOG_INFO, "esl_play_sound start\n");
	char sound_path_name[256];
	sprintf(sound_path_name, "%s/%s", sound_path, sound_file);
	esl_execute(handle, "playback", sound_path_name, NULL);
	esl_log(ESL_LOG_INFO, "esl_play_sound start\n");
}