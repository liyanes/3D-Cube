#include "config.h"
#define _GNU_SOURCE
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ct_ST_config_variable* ct_config_variable_new(const char* name, const char* value)
{
    ct_ST_config_variable* var = (ct_ST_config_variable*)malloc(sizeof(ct_ST_config_variable) + strlen(name) + strlen(value) + 2);
    if (var == NULL)
        return NULL;
    memset((char*)var, 0, sizeof(ct_ST_config_variable));
    TAILQ_INIT(&var->prevline_comment_list);
    TAILQ_INIT(&var->sameline_comment_list);
    var->name = var->buf;
    var->value = var->buf + strlen(name) + 1;
    if (name != NULL)
        strcpy(var->name, name);
    if (value != NULL)
        strcpy(var->value, value);

    return var;
}

void ct_config_variable_destory(ct_ST_config_variable* var)
{
    if (var == NULL)
        return;
    ct_ST_config_comment* comment;
    TAILQ_FOREACH(comment, &var->prevline_comment_list, next)
    {
        if (comment != NULL)
        {
            free(comment->comment);
            free(comment);
        }
    }

    TAILQ_FOREACH(comment, &var->sameline_comment_list, next)
    {
        if (comment != NULL)
        {
            free(comment->comment);
            free(comment);
        }
    }

    free(var);
}

int ct_config_variable_append(ct_ST_config_section* section, ct_ST_config_variable* var)
{
    if (section == NULL || var == NULL)
        return -1;
    //printf("enter ct_config_section_append, %d, %d, %d\n", \
    //       ¡ìion->variable_list, section->variable_list.tqh_last, var);
    TAILQ_INSERT_TAIL(&section->variable_list, var, next);
    return 0;
}

ct_ST_config_variable* ct_config_variable_find(ct_ST_config_section* section, const char* name)
{
    if (section == NULL || name == NULL)
        return NULL;
    ct_ST_config_variable* var;
    TAILQ_FOREACH(var, &section->variable_list, next)
    {
        if (strcmp(var->name, name) == 0)
            return var;
    }
    return NULL;
}

int ct_config_variable_delete(ct_ST_config_section* section, const char* name)
{
    if (section == NULL || name == NULL)
        return -1;

    ct_ST_config_variable* var;
    var = ct_config_variable_find(section, name);
    if (var != NULL)
    {
        TAILQ_REMOVE(&section->variable_list, var, next);
        ct_config_variable_destory(var);
        return 1;
    }
    else
        return 0;

}

void ct_config_variable_display(ct_ST_config_variable* var)
{
    if (var == NULL)
        return NULL;
    printf("        {%s} = {%s} \n", var->name, var->value);
}

ct_ST_config_section* ct_config_section_new(const char* name)
{
    int len = strlen(name);
    ct_ST_config_section* section = malloc(sizeof(ct_ST_config_section) + len + 1);
    section->name = section->buf;
    TAILQ_INIT(&section->variable_list);
    strcpy(section->name, name);

    return section;
}

void ct_config_section_destory(ct_ST_config_section* section)
{
    if (section == NULL)
        return;
    ct_ST_config_variable* var;
    TAILQ_FOREACH(var, &section->variable_list, next)
    {
        ct_config_variable_destory(var);
    }
}

int ct_config_section_append(ct_ST_config* cfg, ct_ST_config_section* section)
{
    if (cfg == NULL || section == NULL)
        return -1;
    //printf("enter ct_config_section_append, %d, %d, %d\n", &(cfg->section_list.tqh_first), cfg->section_list.tqh_last, section);
    TAILQ_INSERT_TAIL(&cfg->section_list, section, next);
    return 0;
}

ct_ST_config_section* ct_config_section_find(ct_ST_config* cfg, const char* match)
{

    if (cfg == NULL || match == NULL)
        return NULL;
    ct_ST_config_section* section;
    TAILQ_FOREACH(section, &cfg->section_list, next)
    {
        if (strcmp(section->name, match) == 0)
            return section;
    }

    return NULL;
}

int ct_config_section_delete(ct_ST_config* cfg, const char* name)
{
    if (cfg == NULL || name == NULL)
        return -1;
    ct_ST_config_section* section;
    section = ct_config_section_find(cfg, name);
    if (section == NULL)
        return 0;
    else
    {
        TAILQ_REMOVE(&cfg->section_list, section, next);
        ct_config_section_destory(section);
        return 0;
    }
}

void ct_config_section_display(ct_ST_config_section* section)
{
    if (section == NULL)
        return;
    ct_ST_config_variable* var;
    printf("     section:%s\n", section->name);
    TAILQ_FOREACH(var, &section->variable_list, next)
    {
        ct_config_variable_display(var);
    }
}

ct_ST_config* ct_config_new(const char* filename)
{

    int len = strlen(filename);
    ct_ST_config* cfg = malloc(sizeof(ct_ST_config) + len + 1);
    TAILQ_INIT(&cfg->section_list);
    cfg->filename = cfg->buf;
    strcpy(cfg->filename, filename);
    //      printf("cfg = %d, cfg->filename = %s, &cfg->section_list = %d, &cfg->section_list.tqh_first = %d, &cfg->section_list.tqh_last = %d\n", \
    //              cfg, cfg->filename, &cfg->section_list, &cfg->section_list.tqh_first, &cfg->section_list.tqh_last);
    return cfg;
}

void ct_config_destory(ct_ST_config* cfg)
{

    if (cfg == NULL)
        return;
    ct_ST_config_section* section;
    TAILQ_FOREACH(section, &cfg->section_list, next)
    {
        ct_config_section_destory(section);
    }
}

static char* find_first_equal(char* string)
{
    if (string == NULL || !strcmp(string, ""))
        return NULL;
    char* comment = NULL, * equal = NULL;
    comment = strchr(string, '#');
    equal = strchr(string, '=');
    if (comment != NULL && comment < equal)
        equal = NULL;
    return equal;
}

static char* find_last_dash(char* string)
{
    if (string == NULL || !strcmp(string, ""))
        return NULL;
    char* comment = NULL, * prev_dash = NULL, * next_dash = NULL;
    comment = strchr(string, '#');

    while ((next_dash = strchr(string, ']')) != NULL)
    {
        if (comment != NULL && next_dash > comment)
            break;
        else
        {
            string = next_dash + 1;
            prev_dash = next_dash;
        }
    }
    return prev_dash;
}

int clear_space(const char* str, char** new_str)
{
    if (str == NULL || !strcmp(str, ""))
        return -1;
    int len;
    const char* start = str, * end = str + strlen(str) - 1;
    while (*start == ' ' || *start == '\n' || *start == '\t')start++;
    while (*end == ' ' || *end == '\n' || *end == '\t')end--;
    if (start > end)
    {
        *new_str = NULL;
        return -1;
    }
    else
    {
        len = end - start + 1;
        *new_str = (char*)malloc(10 + 1);
        strncpy(*new_str, start, len);
        *(*new_str + len) = '\0';
        return 0;
    }
}

static int prase_textline(char* start, ct_ST_config* cfg)
{
    char* p_comment = NULL, * p_equal = NULL, * p_dash = NULL, * name = NULL, * value = NULL,
        * new_name = NULL, * new_value = NULL;
    unsigned long name_len = 0, value_len = 0;
    int ret = 0;
    ct_ST_config_variable* var;
    ct_ST_config_section* section;

    if (start == NULL || !strcmp(start, "") || cfg == NULL)
        return -1;

    if (*start == '#')
    {
        /*process comment*/
    }
    else if (*start == '[')
    {
        /* process section */
        p_comment = strchr(start, '#');
        p_dash = find_last_dash(start + 1);
        if (p_dash == NULL)
        {
            //invalid section
            printf("invalid section,only left dash : %s", start);
            return -1;
        }
        name_len = p_dash - 1 - start;
        //  printf("len = %d, *p_dash = %c\n", name_len, *p_dash);
        name = (char*)malloc(name_len + 1);
        strncpy(name, start + 1, name_len);
        *(name + name_len) = '\0';

        clear_space(name, &new_name);
        if (new_name == NULL || !strcmp(new_name, ""))
        {
            printf("invalid section,no name : %s\n", start);
            return -1;
        }
        // printf("insert new section:%s\n", new_name);
        if (ct_config_section_append(cfg, ct_config_section_new(new_name)) == 0)
        {
            free(name);
            free(new_name);
        }

    }
    else
    {
        /* process variable */
        p_comment = strchr(start, '#');
        p_equal = find_first_equal(start + 1);
        if (p_equal == NULL)
        {
            printf("invalid variable, no equal : %s", start);
            return -1;
        }
        name_len = p_equal - start;
        name = (char*)malloc(name_len + 1);
        if (name == NULL)
            printf("malloc failed\n");
        strncpy(name, start, name_len);
        *(name + name_len) = '\0';
        clear_space(name, &new_name);
        if (new_name == NULL || !strcmp(new_name, ""))
        {
            printf("invalid name : %s", start);
            return -1;
        }

        if (p_comment != NULL)
            value_len = p_comment - p_equal - 1;
        else
            value_len = strlen(p_equal + 1);
        value = (char*)malloc(value_len + 1);
        strncpy(value, p_equal + 1, value_len);
        *(value + value_len) = '\0';
        clear_space(value, &new_value);
        if (new_value == NULL || !strcmp(new_value,value))
        {
            printf("invalid value : %s", start);
            return -1;
        }
        // printf("insert new variable: name = %s, value = %s\n", new_name, new_value);
        section = TAILQ_LAST(&cfg->section_list, ct_config_section_list);
        var = ct_config_variable_new(new_name, new_value);

        ret = ct_config_variable_append(section, var);
        if (ret != 0)
        {
            printf("add variable in section error\n");
            return -1;
        }
    }

    return 0;
}

int ct_config_load(const char* filename, ct_ST_config** cfg)
{
    if (filename == NULL || !strcmp(filename, ""))
        return -1;

    FILE* fp;
    char line[3000], * ptr = NULL;
    size_t len = 0;
    size_t read;

    fp = fopen(filename, "r");
    if (fp == NULL)
        return EXIT_FAILURE;

    *cfg = ct_config_new(filename);
    while (( fgets(line, sizeof(line), fp)) != NULL) {
        //  printf("read text: byte = %zu, len = %d, text = {%s}:\n", read, len, line);
        len = strlen(line);
        ptr = line;

        while (*ptr == ' ') ptr++;

        if (*ptr == "\0")
            continue;

        prase_textline(ptr, *cfg);
    }

    /*if (line)
        free(line);*/
    return 0;
}

void ct_config_display(ct_ST_config* cfg)
{

    ct_ST_config_section* section;
    if (cfg == NULL)
        return;
    printf("----------------------------------- \n");
    printf("    display config parse result\n");
    printf("-----------------------------------\n");
    printf("parse filename : %s\n", cfg->filename);
    TAILQ_FOREACH(section, &cfg->section_list, next)
    {
        ct_config_section_display(section);
    }
}
