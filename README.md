# Season
A small Json library that can only serialize and deserialize json written in C. This library just provide some feature that I need for my project and does not aim to provide all the functionalities that can be expected from a complete Json library (if you're looking for that kind of library look at Json-c).

## Installation
Use the _makefile_ provided in the project to create a static or dynamic library. It may also be interesting to pass the test on the project before using it (it use the [SimpleTest](https://github.com/Cazbrik/SimpleTest) library ):

```sh
git clone https://github.com/Cazbrik/Season.git
cd libtest
git submodule update --init --recursive
make test
make static
make dynamic
```

## Usage
the interface is disponible in the file _src/json.h_ :
```C
#include <stdio.h>

#include "src/json.h"

int main(void){
    int type;
    union json_value value;

    char *json = "{\"foo\": [1, 4, 7, 10], \"bar\": \"baz\"}";
    type = json_parse(&json, &value);
    printf("%d\n", type);

    /* Catch flow from stdout */
    char serialized[1024] = {0};
    freopen("/dev/null", "a", stdout); 
    setbuf(stdout, serialized);

    printf("%d\n", value.object->size);

    type = json_serialize(stdout, value, type);

    /* Redirect */
    freopen("/dev/tty", "a", stdout);

    printf("%s\n", serialized);

    return 0;

}
```