#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "json.h"

#define NULL_POINTER            -3
#define ALLOC_ERR               -2

#define JSON_INVALID            -1
#define JSON_NULL               0
#define JSON_NUMBER             1
#define JSON_STRING             2
#define JSON_OBJECT             3
#define JSON_ARRAY              4

#define NULL_VALUE              "null"

#define NUMBER_MIN              '0'
#define NUMBER_MAX              '9'
#define NUMBER_NEG              '-'
#define NUMBER_POS              '+'

#define STRING_START            '"'
#define STRING_END              '"'

#define OBJECT_START            '{'
#define OBJECT_END              '}'
#define OBJECT_SEP              ','
#define OBJECT_KEY_SEP          ':'


static inline void skip_spaces(char **ptr){
    while(isspace(**ptr)) (*ptr)++;
}

static inline int is_null(char c){
    return c == *NULL_VALUE;
}

static inline int is_number(char c){
    return (c >= NUMBER_MIN && c <= NUMBER_MAX) || c == NUMBER_POS || c == NUMBER_NEG;
}

static inline int is_string(char c){
    return c == STRING_START;
}

static inline int is_object(char c){
    return c == OBJECT_START;
}

static int is_object_empty(char *iter){
    iter++, skip_spaces(&iter);
    return *iter == OBJECT_END;
}

static int get_object_length(char *iter){

    int length = 1;

    while(*iter && *iter != OBJECT_END){
        if(*iter == OBJECT_SEP) length++;
        iter++;
    }
    
    return (*iter == OBJECT_END) ? length : 0;
}

static int attr_parse(char **ptr, struct json_attr *buf){

    int result;
    (*ptr)++, skip_spaces(ptr);
    if(result = string_parse(ptr, &(buf->key)), result != JSON_STRING){
        return result;
    } 

    skip_spaces(ptr);
    if(**ptr != OBJECT_KEY_SEP) {
        free(buf->key);
        return JSON_INVALID;
    }

    (*ptr)++, skip_spaces(ptr);
    if(result = json_parse(ptr, &(buf->value)), result == JSON_INVALID){
        free(buf->key);
        return result;
    } 
    
    buf->type = result;
    return result;

}  

int null_parse(char **ptr){
    int null_len = strlen(NULL_VALUE);
    for(int i = 0; i < null_len; i++, (*ptr)++)
        if(**ptr != NULL_VALUE[i]) return JSON_INVALID;
    return (isalpha(**ptr)) ? JSON_INVALID : JSON_NULL;
}

int number_parse(char **ptr, double *buf){

    if(!buf) return NULL_POINTER;
    if(!is_number(**ptr)) return JSON_INVALID;

    char *end = *ptr;
    *buf = strtod(*ptr, &end);

    return *ptr = end, JSON_NUMBER;

}

int string_parse(char **ptr, char **buf){

    if(!buf) return NULL_POINTER;
    if(!is_string(**ptr)) return JSON_INVALID;

    const char *start = ++(*ptr);
    while(**ptr && **ptr != STRING_END) (*ptr)++;
    if(!**ptr) return JSON_INVALID;

    (*ptr)++;
    *buf = calloc((*ptr - start), sizeof(char));
    if(!*buf) return ALLOC_ERR;

    memset(*buf, 0, sizeof(char) * (*ptr - start));
    strncpy(*buf, start, (*ptr - start) - 1);

    return JSON_STRING;

}

int object_parse(char **ptr, struct json_object **buf){

    if(!buf) return NULL_POINTER;
    if(!is_object(**ptr)) return JSON_INVALID;
    
    if(is_object_empty(*ptr)){
        *buf = object_create(0);
        return (!*buf) ? ALLOC_ERR : JSON_OBJECT; 
    }
    int length = get_object_length(*ptr);
    if(!length) return JSON_INVALID;
    
    *buf = object_create(length);
    if(!*buf) return ALLOC_ERR; 

    int counter = 0, result;
    do{

        result = attr_parse(ptr, &((*buf)->values[counter]));
        if(result == JSON_INVALID){
            (*buf)->length = counter - 1;
            object_dispose(*buf);
            return result;
        }
        skip_spaces(ptr);
        counter += 1;

    } while (**ptr == OBJECT_SEP);

    if(**ptr != OBJECT_END){
        object_dispose(*buf);
        return JSON_INVALID;
    }

    return JSON_OBJECT;

}

int json_parse(char **ptr, union json_value *buf){
    
    if(!ptr || !buf) return NULL_POINTER;

    skip_spaces(ptr);
    int result;

    if(is_null(**ptr)){
       
       return null_parse(ptr);

    } else if(is_number(**ptr)){

        return number_parse(ptr, &(buf->number));

    } else if(is_string(**ptr)){

        return string_parse(ptr, &(buf->string));

    } else if(is_object(**ptr)){
        
        return object_parse(ptr, &(buf->object));

    } else {

        return JSON_INVALID;

    }

}

struct json_object *object_create(int length){

    if(length < 0) return NULL;

    struct json_object *obj = malloc(sizeof(struct json_object));
    if(!obj) return NULL;
    memset(obj, 0, sizeof(struct json_object));

    if(length){
        obj->length = length;
        obj->values = calloc(length, sizeof(struct json_attr));
        if(!obj->values) return free(obj), NULL; 
    }

    return obj;

}

void object_dispose(struct json_object* obj){

    for(int i = 0; i < obj->length; i++){
        
        if(obj->values[i].type == JSON_OBJECT){
            object_dispose(obj->values[i].value.object);
        } else if (obj->values[i].type == JSON_STRING){
            free(obj->values[i].value.string);
        }

        free(obj->values[i].key);

    }
    
    free(obj);

}