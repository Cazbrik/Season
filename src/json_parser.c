#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "json.h"

static inline void skip_spaces(char **ptr){
    while(isspace(**ptr)) (*ptr)++;
}

static inline int is_null(char c){
    return c == *JSON_NULL_VALUE;
}

static inline int is_number(char c){
    return (c >= JSON_NUMBER_MIN && c <= JSON_NUMBER_MAX) || c == JSON_NUMBER_POS || c == JSON_NUMBER_NEG;
}

static inline int is_string(char c){
    return c == JSON_STRING_START;
}

static inline int is(char c, char expected){
    return c == expected;
}

static int is_empty(char *iter, char end){
    iter++, skip_spaces(&iter);
    return *iter == end;
}

static void move_end(char **ptr, char end){
    (*ptr)++, skip_spaces(ptr), (*ptr)++;
}

static void move_to(char **ptr, char end){
    while(**ptr && **ptr != end) (*ptr)++;
    
}

static int get_size(char *iter, char end, char sep, char start){

    int size = 1;
    iter++;
    while(*iter && *iter != end){
        if(*iter == sep) size++;
        else if(*iter == JSON_ARRAY_START) move_to(&iter, JSON_ARRAY_END);
        else if(*iter == JSON_OBJECT_START) move_to(&iter, JSON_OBJECT_END);
        iter++;
    }
    
    return (*iter == end) ? size : 0;
}

static int object_attr_parse(char **ptr, struct json_attr *buf){

    int result;

    (*ptr)++, skip_spaces(ptr);
    if(result = json_string_parse(ptr, &(buf->key)), result != JSON_STRING){
        return result;
    } 

    skip_spaces(ptr);
    if(**ptr != JSON_OBJECT_KEY_SEP) {
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

static int array_attr_parse(char **ptr, struct json_attr *buf){

    int result;

    (*ptr)++, skip_spaces(ptr);
    if(result = json_parse(ptr, &(buf->value)), result == JSON_INVALID){
        return result;
    } 
    
    buf->type = result;
    return result;

}

int json_null_parse(char **ptr){
    if(!ptr) return NULL_POINTER;
    int null_len = strlen(JSON_NULL_VALUE);
    for(int i = 0; i < null_len; i++, (*ptr)++)
        if(**ptr != JSON_NULL_VALUE[i]) return JSON_INVALID;
    return (isalpha(**ptr)) ? JSON_INVALID : JSON_NULL;
}

int json_number_parse(char **ptr, double *buf){

    if(!buf || !ptr) return NULL_POINTER;
    if(!is_number(**ptr)) return JSON_INVALID;

    char *end = *ptr;
    *buf = strtod(*ptr, &end);

    return *ptr = end, JSON_NUMBER;

}

int json_string_parse(char **ptr, char **buf){

    if(!buf || !ptr) return NULL_POINTER;
    if(!is_string(**ptr)) return JSON_INVALID;

    const char *start = ++(*ptr);
    while(**ptr && **ptr != JSON_STRING_END) (*ptr)++;
    if(!**ptr) return JSON_INVALID;

    (*ptr)++;
    *buf = calloc((*ptr - start), sizeof(char));
    if(!*buf) return ALLOC_ERR;

    memset(*buf, 0, sizeof(char) * (*ptr - start));
    strncpy(*buf, start, (*ptr - start) - 1);

    return JSON_STRING;

}

// It's looooooooooonng
static int array_or_object_parse(char **ptr, struct json **buf, char start, char sep, char end, int success,
int(*attr_parse)(char **, struct json_attr *)){

    if(!buf || !ptr) return NULL_POINTER;
    if(!is(**ptr, start)) return JSON_INVALID;

    if(is_empty(*ptr, end)){
        move_end(ptr, end);
        *buf = json_create(0);
        return  (!(*buf)) ? ALLOC_ERR : success; 
    }

    int size = get_size(*ptr, end, sep, start);
    if(!size) return JSON_INVALID;
    *buf = json_create(size);
    if(!(*buf)) return ALLOC_ERR;
    
    int counter = 0, result;

    do{

        result = attr_parse(ptr, &((*buf)->values[counter]));
        if(result == JSON_INVALID){
            (*buf)->size = counter - 1;
            json_dispose(*buf);
            return result;
        }
        skip_spaces(ptr);
        counter += 1;

    } while (**ptr == sep);

    if(**ptr != end){
        json_dispose(*buf);
        return JSON_INVALID;
    }

    return (*ptr)++, success;

}

int json_object_parse(char **ptr, struct json **buf){
    return array_or_object_parse(ptr, buf, JSON_OBJECT_START, JSON_OBJECT_SEP, JSON_OBJECT_END, JSON_OBJECT, object_attr_parse);
}

int json_array_parse(char **ptr, struct json **buf){
    return array_or_object_parse(ptr, buf, JSON_ARRAY_START, JSON_ARRAY_SEP, JSON_ARRAY_END, JSON_ARRAY, array_attr_parse);
}

int json_parse(char **ptr, union json_value *buf){
    
    if(!ptr || !buf) return NULL_POINTER;

    skip_spaces(ptr);
    int result;

    if(is_null(**ptr)){
       
       return json_null_parse(ptr);

    } else if(is_number(**ptr)){

        return json_number_parse(ptr, &(buf->number));

    } else if(is_string(**ptr)){

        return json_string_parse(ptr, &(buf->string));

    } else if(is(**ptr, JSON_OBJECT_START)){
        
        return json_object_parse(ptr, &(buf->object));

    } else if(is(**ptr, JSON_ARRAY_START)){
        
        return json_array_parse(ptr, &(buf->object));

    } else {

        return JSON_INVALID;

    }

}

struct json *json_create(int size){

    if(size < 0) return NULL;

    struct json *ptr = malloc(sizeof(struct json));
    if(!ptr) return NULL;
    memset(ptr, 0, sizeof(struct json));

    if(size){
        ptr->size = size;
        ptr->values = calloc(size, sizeof(struct json_attr));
        if(!ptr->values) return free(ptr), NULL; 
    }

    return ptr;

}

void json_dispose(struct json *obj){

    for(int i = 0; i < obj->size; i++){

        if(obj->values[i].type == JSON_OBJECT || obj->values[i].type == JSON_ARRAY){
            json_dispose(obj->values[i].value.object);
        } else if (obj->values[i].type == JSON_STRING){
            free(obj->values[i].value.string);
        }

        if(obj->values[i].key) free(obj->values[i].key);

    }
    
    free(obj);

}