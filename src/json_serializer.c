#include <string.h>

#include "json.h"

#define END_SPACE   " "

void json_null_serialize(FILE *file){
    fprintf(file, JSON_NULL_VALUE);
}

void json_number_serialize(FILE *file, double buf){
    fprintf(file, "%.20g", buf);
}

void json_string_serialize(FILE *file, char *buf){
    fprintf(file, "%c%s%c", JSON_STRING_START, buf, JSON_STRING_END);
}

void json_object_serialize(FILE *file, struct json *buf){
    
}

void json_array_serialize(FILE *file, struct json *buf);

void json_serialize(FILE *file, union json_value buf);