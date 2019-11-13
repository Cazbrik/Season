#ifndef JSON_H
#define JSON_H

union json_value {
    double number;
    char *string;
    struct json_object *object;
};

struct json_attr {
    int type;
    char *key;
    union json_value value;
};

struct json_object {
    int length;
    struct json_attr *values;
};

int json_null_parse(char **);

int json_number_parse(char **, double *);

int json_string_parse(char **, char **);

int json_object_parse(char **, struct json_object **);

int json_parse(char **, union json_value *);

struct json_object *json_object_create(int);

void json_object_dispose(struct json_object *);

#endif