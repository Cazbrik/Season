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

int null_parse(char **);

int number_parse(char **, double *);

int string_parse(char **, char **);

int object_parse(char **, struct json_object **);

int json_parse(char **, union json_value *);

struct json_object *object_create(int);

void object_dispose(struct json_object *);

#endif