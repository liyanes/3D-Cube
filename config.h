#pragma once
#ifndef __CONFIG_H__
#define __CONFIG_H__

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

#include "list.h"

	struct ct_config_comment
	{
		TAILQ_ENTRY(ct_config_comment) next;
		char comment[0];
	};

	typedef struct ct_config_comment ct_ST_config_comment;

	struct ct_config_variable
	{
		TAILQ_ENTRY(ct_config_variable) next;
		char* name;
		char* value;
		//int line;
		TAILQ_HEAD(ct_config_comment_list, ct_config_comment) prevline_comment_list, sameline_comment_list;
		//TAILQ_HEAD(ct_config_comment_list, ct_config_comment) sameline_comment_list;
		int blanklines;
		char buf[0];
	};

	typedef struct ct_config_variable ct_ST_config_variable;

	struct ct_config_section
	{
		TAILQ_ENTRY(ct_config_section) next;
		char* name;
		TAILQ_HEAD(ct_config_variable_list, ct_config_variable) variable_list;
		//TAILQ_HEAD(ct_config_comment_list, ct_config_comment) prevline_comment_list, sameline_comment_list;
		//TAILQ_HEAD(ct_config_comment_list, ct_config_comment) sameline_comment_list;
		int blanklines;
		char buf[0];
	};

	typedef struct ct_config_section ct_ST_config_section;

	struct ct_config
	{
		char* filename;
		TAILQ_HEAD(ct_config_section_list, ct_config_section) section_list;
		char buf[0];
	};

	typedef struct ct_config ct_ST_config;

	//function for config
	/*
	ct_ST_config_comment *ct_config_comment_new(const char *comment);
	int ct_config_comment_destory(ct_ST_config_comment *p);
	int ct_config_comment_modify(ct_ST_config *config, const char *section_name, const char *variable_name, const char *comment);
	*/

	ct_ST_config_variable* ct_config_variable_new(const char* name, const char* value);
	void ct_config_variable_destory(ct_ST_config_variable* var);
	int ct_config_variable_append(ct_ST_config_section* section, ct_ST_config_variable* var);
	int ct_config_variable_delete(ct_ST_config_section* section, const char* name);
	int ct_config_variable_update(ct_ST_config_section* section, const char* name, const char* match, const char* new_value);
	ct_ST_config_variable* ct_config_variable_find(ct_ST_config_section* section, const char* name);
	void ct_config_variable_display(ct_ST_config_variable* var);

	ct_ST_config_section* ct_config_section_new(const char* name);
	void ct_config_section_destory(ct_ST_config_section* section);
	int ct_config_section_append(ct_ST_config* cfg, ct_ST_config_section* section);
	int ct_config_section_delete(ct_ST_config* cfg, const char* name);
	int ct_config_section_update(ct_ST_config* cfg, const char* match, const char* new_name);
	ct_ST_config_section* ct_config_section_find(ct_ST_config* cfg, const char* match);
	void ct_config_section_display(ct_ST_config_section* section);

	//ct_ST_config *ct_config_new();
	ct_ST_config* ct_config_new(const char* filename);
	void ct_config_destory(ct_ST_config* cfg);
	int ct_config_load(const char* filename, ct_ST_config** cfg);
	int ct_config_save(ct_ST_config* cfg, const char* new_filename);
	void ct_config_display(ct_ST_config* cfg);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
