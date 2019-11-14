#ifndef JSON_H
#define JSON_H

#include <stdio.h>

/* Code that may be returned by operation */

#define NULL_POINTER                    -11
#define ALLOC_ERR                       -10

/* Json type used to parse Json */

#define JSON_INVALID                    -1
#define JSON_NULL                       0
#define JSON_NUMBER                     1
#define JSON_STRING                     2
#define JSON_OBJECT                     3
#define JSON_ARRAY                      4

/* Macro to define key character of a Json */

#define JSON_NULL_VALUE                 "null"

#define JSON_NUMBER_MIN                 '0'
#define JSON_NUMBER_MAX                 '9'
#define JSON_NUMBER_NEG                 '-'
#define JSON_NUMBER_POS                 '+'

#define JSON_STRING_START               '"'
#define JSON_STRING_END                 '"'

#define JSON_OBJECT_START               '{'
#define JSON_OBJECT_END                 '}'
#define JSON_OBJECT_SEP                 ','
#define JSON_OBJECT_KEY_SEP             ':'

#define JSON_ARRAY_START                '['
#define JSON_ARRAY_END                 ']'
#define JSON_ARRAY_SEP                  ','

/* Structure used to represent a Json object */

struct json {
    int size;
    struct json_attr *values;
};

union json_value {
    double number;
    char *string;
    struct json *object;
};


struct json_attr {
    int type;
    char *key; /* /!\ For array this field should be null /!\ */
    union json_value value;
};

/* ----- Parse operation ----- */

int json_null_parse(char **ptr);

int json_number_parse(char **ptr, double *buf);

int json_string_parse(char **ptr, char **buf);

int json_object_parse(char **ptr, struct json **buf);

int json_array_parse(char **ptr, struct json **buf);

int json_parse(char **ptr, union json_value *buf);

/* ----- Serialize operation ----- */

int json_null_serialize(FILE *file);

int json_number_serialize(FILE *file, double buf);

int json_string_serialize(FILE *file, char *buf);

int json_object_serialize(FILE *file, struct json *buf);

int json_array_serialize(FILE *file, struct json *buf);

int json_serialize(FILE *file, union json_value buf, int type);

/* ----- Struct operation ----- */

struct json *json_create(int);

void json_dispose(struct json *);

#endif