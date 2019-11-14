#include <string.h>

#include "json.h"

#define SPACE   " "

int json_null_serialize(FILE *file){
    fprintf(file, JSON_NULL_VALUE);
    return JSON_NULL;
}

int json_number_serialize(FILE *file, double buf){

    fprintf(file, "%.6g", buf);
    return JSON_NUMBER;
}

int json_string_serialize(FILE *file, char *buf){
    if(!buf) return JSON_INVALID;
    fprintf(file, "%c%s%c", JSON_STRING_START, buf, JSON_STRING_END);
    return JSON_STRING;
}

static int json_attr_serialize(FILE *file, struct json_attr *buf){

    if(!buf) return JSON_INVALID;

    if(buf->key){
        json_string_serialize(file, buf->key);
        fprintf(file, SPACE "%c" SPACE, JSON_OBJECT_KEY_SEP);
    }

   json_serialize(file, buf->value, buf->type);

}

static int array_or_object_serialize(FILE *file, struct json *buf, char start, char sep, char end, int success){

    if(!buf) return NULL_POINTER;
    if(buf->size < 0) return JSON_INVALID;

    struct json_attr attr;
    int result;
    fprintf(file, "%c", start);

    if(buf->size){

        if(!(buf->values)) return JSON_INVALID;
        for(int i = 0; i < buf->size; i++){

            attr = buf->values[i];
            fprintf(file, SPACE);
            if(result = json_attr_serialize(file, &attr), result < 0) return result;
            fprintf(file, SPACE);
            if(i + 1 < buf->size) fprintf(file, "%c", sep);

        }

    }

    fprintf(file, "%c", end);

    return success;
}

int json_object_serialize(FILE *file, struct json *buf){
    return array_or_object_serialize(file, buf, JSON_OBJECT_START, JSON_OBJECT_SEP, JSON_OBJECT_END, JSON_OBJECT);
}

int json_array_serialize(FILE *file, struct json *buf){
    return array_or_object_serialize(file, buf, JSON_ARRAY_START, JSON_ARRAY_SEP, JSON_ARRAY_END, JSON_ARRAY);
}

int json_serialize(FILE *file, union json_value buf, int type){
     switch(type){
        case JSON_NULL : return json_null_serialize(file);
        case JSON_NUMBER : return json_number_serialize(file, buf.number);
        case JSON_STRING : return json_string_serialize(file, buf.string);
        case JSON_OBJECT : return  json_object_serialize(file, buf.object);
        case JSON_ARRAY : return  json_array_serialize(file, buf.object);
        default : return JSON_INVALID;
    }
}