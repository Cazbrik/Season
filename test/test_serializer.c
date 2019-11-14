#include "../libtest/src/test.h"
#include "../src/json_serializer.c"
#include "test_parser.c"


#undef BEFORE_TEST
#define BEFORE_TEST char string[TEST_SIZE]; freopen("/dev/null", "a", stdout); setbuf(stdout, string);

#undef AFTER_TEST 
#define AFTER_TEST freopen("/dev/tty", "a", stdout);

#define TEST_SIZE 4
TEST(test_json_null_serialize, {
    json_null_serialize(stdout);
    AFTER_TEST;
    assert_string(JSON_NULL_VALUE, string);
});

#undef TEST_SIZE
#define TEST_SIZE 2
TEST(test_json_number_serialize, {
    json_number_serialize(stdout, 45);
    AFTER_TEST;
    assert_string("45", string);
});

void test_serializer(){
    test_json_null_serialize();
    test_json_number_serialize();
}

int main(void){
    // test_parser();
    test_serializer();
    test_result();
    return 0;
}