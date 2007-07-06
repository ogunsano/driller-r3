#include <copper.hpp>
#include "../src/database/misc.h"

using namespace Driller;

TEST_SUITE(misc_tests) {

TEST(integer_sizes) {
  ASSERT(equal(1u, sizeof(int8)));
  ASSERT(equal(1u, sizeof(uint8)));
  ASSERT(equal(2u, sizeof(int16)));
  ASSERT(equal(2u, sizeof(uint16)));
  ASSERT(equal(4u, sizeof(int32)));
  ASSERT(equal(4u, sizeof(uint32)));
}

TEST(column_strings) {
  ASSERT(equal("Unknown", column_strings[COLUMN_UNKNOWN]));

  ASSERT(equal("Boolean", column_strings[COLUMN_BOOL]));

  ASSERT(equal("Signed 8-bit integer", column_strings[COLUMN_INT8]));
  ASSERT(equal("Unsigned 8-bit integer", column_strings[COLUMN_UINT8]));
  ASSERT(equal("Signed 16-bit integer", column_strings[COLUMN_INT16]));
  ASSERT(equal("Unsigned 16-bit integer", column_strings[COLUMN_UINT16]));
  ASSERT(equal("Signed 32-bit integer", column_strings[COLUMN_INT32]));
  ASSERT(equal("Unsigned 32-bit integer", column_strings[COLUMN_UINT32]));

  ASSERT(equal("Binary blob", column_strings[COLUMN_BLOB]));
  ASSERT(equal("Fixed-length string", column_strings[COLUMN_STRING]));
  ASSERT(equal("Variable-length string", column_strings[COLUMN_VARSTRING]));

  ASSERT(equal("Phone number", column_strings[COLUMN_PHONE]));
  ASSERT(equal("Date", column_strings[COLUMN_DATE]));
  ASSERT(equal("Currency", column_strings[COLUMN_CURRENCY]));
  ASSERT(equal("Enumeration", column_strings[COLUMN_ENUM]));
}

TEST(short_column_strings) {
  ASSERT(equal("unknown", short_column_strings[COLUMN_UNKNOWN]));

  ASSERT(equal("bool", short_column_strings[COLUMN_BOOL]));

  ASSERT(equal("int8", short_column_strings[COLUMN_INT8]));
  ASSERT(equal("uint8", short_column_strings[COLUMN_UINT8]));
  ASSERT(equal("int16", short_column_strings[COLUMN_INT16]));
  ASSERT(equal("uint16", short_column_strings[COLUMN_UINT16]));
  ASSERT(equal("int32", short_column_strings[COLUMN_INT32]));
  ASSERT(equal("uint32", short_column_strings[COLUMN_UINT32]));

  ASSERT(equal("blob", short_column_strings[COLUMN_BLOB]));
  ASSERT(equal("string", short_column_strings[COLUMN_STRING]));
  ASSERT(equal("varstring", short_column_strings[COLUMN_VARSTRING]));

  ASSERT(equal("phone", short_column_strings[COLUMN_PHONE]));
  ASSERT(equal("date", short_column_strings[COLUMN_DATE]));
  ASSERT(equal("currency", short_column_strings[COLUMN_CURRENCY]));
  ASSERT(equal("enum", short_column_strings[COLUMN_ENUM]));
}

}
