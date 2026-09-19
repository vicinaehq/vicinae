file(READ "${INPUT}" HEX_CONTENT HEX)
string(LENGTH "${HEX_CONTENT}" HEX_LENGTH)
math(EXPR BYTE_COUNT "${HEX_LENGTH} / 2")
string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," ARRAY_BODY "${HEX_CONTENT}")
file(WRITE "${OUTPUT}"
"#include <cstddef>
extern const unsigned char ${VAR}[] = {${ARRAY_BODY}};
extern const std::size_t ${VAR}_SIZE = ${BYTE_COUNT};
")
