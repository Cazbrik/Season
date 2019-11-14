#include "../libtest/src/test.h"
#include "../src/json_parser.c"

/* Test skip_spaces() operation */

TEST(test_skip_spaces, {

    char *str = "\t\t\t\ts";
    skip_spaces(&str);
    assert_int('s' , *str);

    str = "\n\n\n\ns";
    skip_spaces(&str);
    assert_int('s' , *str);

    str = "    s";
    skip_spaces(&str);
    assert_int('s' , *str);

    str = "s";
    skip_spaces(&str);
    assert_int('s' , *str);

    str = "";
    skip_spaces(&str);
    assert_int('\0' , *str);

});

/* Test test_json_null_parse() operation */

TEST(test_json_null_parse, {

    char *str = "null";
    assert_int(JSON_NULL, json_null_parse(&str));

    str = "nul";
    assert_int(JSON_INVALID, json_null_parse(&str));

    str = "nulll";
    assert_int(JSON_INVALID, json_null_parse(&str));

});

/* Test json_number_parse() operation */

TEST(test_json_number_parse, {

    double buf;
    char *str = "-45";
    assert_int(JSON_NUMBER, json_number_parse(&str, &buf));
    assert_double(-45, buf);
    assert_int(0, *str);

    str = "34.3";
    assert_int(JSON_NUMBER, json_number_parse(&str, &buf));
    assert_double(34.3, buf);
    assert_int(0, *str);

    str = "0";
    assert_int(JSON_NUMBER, json_number_parse(&str, &buf));
    assert_double(0, buf);
    assert_int(0, *str);

});

/* Test json_string_parse() operation */

TEST(test_json_string_parse, {

    char *buf;
    char *str = "\"String\"";
    assert_int(JSON_STRING, json_string_parse(&str, &buf));
    assert_string("String", buf);
    assert_int(0, *str);
    free(buf);

    str = "\"String";
    assert_int(JSON_INVALID, json_string_parse(&str, &buf));

    str = "String\"";
    assert_int(JSON_INVALID, json_string_parse(&str, &buf));

});

/* Test json_object_parse() operation */

TEST(test_json_object_parse, {
    
    struct json *buf;

    char *str = "{}";
    assert_int(JSON_OBJECT, json_object_parse(&str, &buf));
    assert_int(0, buf->size);
    assert_null(buf->values);
    json_dispose(buf);

    str = "{ \"key\" : 23 }";
    assert_int(JSON_OBJECT, json_object_parse(&str, &buf));
    assert_int(1, buf->size);

    assert_int(JSON_NUMBER, buf->values[0].type);
    assert_string("key" , buf->values[0].key);
    assert_double(23, buf->values[0].value.number);
    json_dispose(buf);

    str = "{ \"key\" : 23, \"key2\" : \"String\"}";
    assert_int(JSON_OBJECT, json_object_parse(&str, &buf));
    assert_int(2, buf->size);

    assert_int(JSON_NUMBER, buf->values[0].type);
    assert_string("key" , buf->values[0].key);
    assert_double(23, buf->values[0].value.number);

    assert_int(JSON_STRING, buf->values[1].type);
    assert_string("key2" , buf->values[1].key);
    assert_string("String", buf->values[1].value.string);
    json_dispose(buf);

    str = "{ \"key\" : 23 \"key2\" : \"String\"}";
    assert_int(JSON_INVALID, json_object_parse(&str, &buf));

    str = "{ \"key\" : 23, \"key2\"  \"String\"}";
    assert_int(JSON_INVALID, json_object_parse(&str, &buf));

    str = "{ \"key\" : {}, \"key2\" : \"String\"}";
    assert_int(JSON_OBJECT, json_object_parse(&str, &buf));
    assert_int(2, buf->size);
    json_dispose(buf);

});

/* Test test_json_array_parse() operation */

TEST(test_json_array_parse, {
    
    struct json *buf;

    char *str = "[]";
    assert_int(JSON_ARRAY, json_array_parse(&str, &buf));
    assert_int(0, buf->size);
    assert_null(buf->values);
    json_dispose(buf);

    str = "[ \"key\", 23, null, {\"simple_key\" : \"simple_value\" }, [] ]";
    assert_int(JSON_ARRAY, json_array_parse(&str, &buf));
    assert_int(5, buf->size);

    assert_int(JSON_STRING, buf->values[0].type);
    assert_string("key" , buf->values[0].value.string);

    assert_int(JSON_NUMBER, buf->values[1].type);
    assert_int(23 , buf->values[1].value.number);

    assert_int(JSON_NULL, buf->values[2].type);

    assert_int(JSON_OBJECT, buf->values[3].type);
    assert_int(1 , buf->values[3].value.object->size);
    assert_int(JSON_STRING , buf->values[3].value.object->values[0].type);
    assert_string("simple_key" , buf->values[3].value.object->values[0].key);
    assert_string("simple_value" , buf->values[3].value.object->values[0].value.string);

    assert_int(JSON_ARRAY, buf->values[4].type);
    assert_int(0 , buf->values[4].value.object->size);
    json_dispose(buf);

    str = "[23  \"String\"]";
    assert_int(JSON_INVALID, json_array_parse(&str, &buf));

    str = "[ 23,\"String\"";
    assert_int(JSON_INVALID, json_array_parse(&str, &buf));


    str = "[[], 12]";
    assert_int(JSON_ARRAY, json_array_parse(&str, &buf));
    assert_int(2, buf->size);
    json_dispose(buf);

});

/* Test json_parse() operation */

TEST(test_json_parse, {

    union json_value buf;

    char *str = "null";
    assert_int(JSON_NULL, json_parse(&str, &buf));

    str = "23";
    assert_int(JSON_NUMBER, json_parse(&str, &buf));

    str = "\"String\"";
    assert_int(JSON_STRING, json_parse(&str, &buf));
    free(buf.string);

    str = "{}";
    assert_int(JSON_OBJECT, json_parse(&str, &buf));
    free(buf.object);

    str = "{\"key_null\" : null,\"key_string\" : \"String\", \"key_number\" : 12,\"key_object\" : { \"simple_key\" : \"simple_object\" }}";
    assert_int(JSON_OBJECT, json_parse(&str, &buf));
    assert_int(4, buf.object->size);

    assert_int(JSON_NULL, buf.object->values[0].type);
    assert_string("key_null", buf.object->values[0].key);

    assert_int(JSON_STRING, buf.object->values[1].type);
    assert_string("key_string", buf.object->values[1].key);
    assert_string("String", buf.object->values[1].value.string);

    assert_int(JSON_NUMBER, buf.object->values[2].type);
    assert_string("key_number", buf.object->values[2].key);
    assert_int(12, buf.object->values[2].value.number);

    assert_int(JSON_OBJECT, buf.object->values[3].type);
    assert_string("key_object", buf.object->values[3].key);


    assert_int(1, buf.object->values[3].value.object->size);
    assert_string("simple_key", buf.object->values[3].value.object->values[0].key);
    assert_string("simple_object", buf.object->values[3].value.object->values[0].value.string);
    free(buf.object);

});

void test_parser(){
    test_skip_spaces();
    test_json_null_parse();
    test_json_number_parse();
    test_json_string_parse();
    test_json_object_parse();
    test_json_array_parse();
    test_json_parse();
}