#include "../libtest/src/test.h"
#include "../src/json_serializer.c"
#include "test_parser.c"

#define TEST_SIZE 1024

#undef BEFORE_TEST
#define BEFORE_TEST char string[TEST_SIZE] = {0}; freopen("/dev/null", "a", stdout); setbuf(stdout, string);

#define RESET_STDOUT freopen("/dev/tty", "a", stdout);
#define RE_SET_STDOUT memset(string, 0, sizeof(string)); freopen("/dev/null", "a", stdout); setbuf(stdout, string);

TEST(test_json_null_serialize, {
    int result =  json_null_serialize(stdout);
    RESET_STDOUT;
    assert_int(JSON_NULL, result);
    assert_string(JSON_NULL_VALUE, string);
});

TEST(test_json_number_serialize, {
    int result;

    result = json_number_serialize(stdout, 45.4);
    RESET_STDOUT;
    assert_int(JSON_NUMBER, result);
    assert_string("45.4", string);

    RE_SET_STDOUT
    result = json_number_serialize(stdout, -45.4);
    RESET_STDOUT;
    assert_int(JSON_NUMBER, result);
    RESET_STDOUT;
    assert_string("-45.4", string);

    RE_SET_STDOUT
    result = json_number_serialize(stdout, 0);
    RESET_STDOUT;
    assert_int(JSON_NUMBER, result);
    RESET_STDOUT;
    assert_string("0", string);

    RE_SET_STDOUT
    result = json_number_serialize(stdout, 4);
    RESET_STDOUT;
    assert_int(JSON_NUMBER, result);
    assert_string("4", string);

});

TEST(test_json_string_serialize, {

    int result = json_string_serialize(stdout, "String");
    RESET_STDOUT;
    assert_int(JSON_STRING, result);
    assert_string("\"String\"", string);

    RE_SET_STDOUT
    result = json_string_serialize(stdout, "Compl3X3_5tr1n6$__");
    RESET_STDOUT;
    assert_int(JSON_STRING, result);
    assert_string("\"Compl3X3_5tr1n6$__\"", string);

    RE_SET_STDOUT
    result =  json_string_serialize(stdout, NULL);
    RESET_STDOUT;
    assert_int(JSON_INVALID, result);

});

TEST(test_json_object_serialize, {

    union json_value value;
    struct json empty;
    struct json_attr attr;
    struct json obj;

    value.string = "String";
    attr.type = JSON_STRING; attr.key = "key"; attr.value = value;
    obj.size = 1; obj.values = &attr;
    
    int result = json_object_serialize(stdout, &obj);
    RESET_STDOUT;
    assert_int(JSON_OBJECT, result);
    assert_string("{ \"key\" : \"String\" }" , string);

    RE_SET_STDOUT
    value.number = 12;
    attr.type = JSON_NUMBER; attr.key = "key"; attr.value = value;
    obj.size = 1; obj.values = &attr;
    result = json_object_serialize(stdout, &obj);
    RESET_STDOUT;
    assert_int(JSON_OBJECT, result);
    assert_string("{ \"key\" : 12 }" , string);

    RE_SET_STDOUT
    attr.type = JSON_NULL;
    result = json_object_serialize(stdout, &obj);
    RESET_STDOUT;
    assert_int(JSON_OBJECT, result);
    assert_string("{ \"key\" : null }" , string);

    RE_SET_STDOUT
    empty.size = 0;
    empty.values = NULL;
    value.object = &empty ;
    attr.type = JSON_ARRAY; attr.key = "key"; attr.value = value;
    obj.size = 1; obj.values = &attr;
    result = json_object_serialize(stdout, &obj);
    RESET_STDOUT;
    assert_int(JSON_OBJECT, result);
    assert_string("{ \"key\" : [] }" , string);

    RE_SET_STDOUT
    attr.type = JSON_OBJECT;
    result = json_object_serialize(stdout, &obj);
    RESET_STDOUT;
    assert_int(JSON_OBJECT, result);
    assert_string("{ \"key\" : {} }" , string);

    RE_SET_STDOUT
    result = json_object_serialize(stdout, NULL);
    RESET_STDOUT;
    assert_int(NULL_POINTER, result);

});

void test_serializer(){

    test_json_null_serialize();
    test_json_number_serialize();
    test_json_string_serialize();
    test_json_object_serialize();
}

int main(void){
    test_parser();
    test_serializer();
    test_result();
    return 0;
}