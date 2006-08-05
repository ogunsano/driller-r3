#include "test.h"
#include "database/misc.h"

using namespace Driller;

TEST_SUITE(misc_tests)

TEST(integer_sizes)
  assert(sizeof(int8)).equals(1);
  assert(sizeof(uint8)).equals(1);
  assert(sizeof(int16)).equals(2);
  assert(sizeof(uint16)).equals(2);
  assert(sizeof(int32)).equals(4);
  assert(sizeof(uint32)).equals(4);
}

TEST(column_strings)
  assert(column_strings[COLUMN_UNKNOWN]).equals("Unknown");

  assert(column_strings[COLUMN_BOOL]).equals("Boolean");

  assert(column_strings[COLUMN_INT8]).equals("Signed 8-bit integer");
  assert(column_strings[COLUMN_UINT8]).equals("Unsigned 8-bit integer");
  assert(column_strings[COLUMN_INT16]).equals("Signed 16-bit integer");
  assert(column_strings[COLUMN_UINT16]).equals("Unsigned 16-bit integer");
  assert(column_strings[COLUMN_INT32]).equals("Signed 32-bit integer");
  assert(column_strings[COLUMN_UINT32]).equals("Unsigned 32-bit integer");

  assert(column_strings[COLUMN_BLOB]).equals("Binary blob");
  assert(column_strings[COLUMN_STRING]).equals("Fixed-length string");
  assert(column_strings[COLUMN_VARSTRING]).equals("Variable-length string");

  assert(column_strings[COLUMN_PHONE]).equals("Phone number");
  assert(column_strings[COLUMN_DATE]).equals("Date");
  assert(column_strings[COLUMN_CURRENCY]).equals("Currency");
  assert(column_strings[COLUMN_ENUM]).equals("Enumeration");
}

TEST(short_column_strings)
  assert(short_column_strings[COLUMN_UNKNOWN]).equals("unknown");

  assert(short_column_strings[COLUMN_BOOL]).equals("bool");

  assert(short_column_strings[COLUMN_INT8]).equals("int8");
  assert(short_column_strings[COLUMN_UINT8]).equals("uint8");
  assert(short_column_strings[COLUMN_INT16]).equals("int16");
  assert(short_column_strings[COLUMN_UINT16]).equals("uint16");
  assert(short_column_strings[COLUMN_INT32]).equals("int32");
  assert(short_column_strings[COLUMN_UINT32]).equals("uint32");

  assert(short_column_strings[COLUMN_BLOB]).equals("blob");
  assert(short_column_strings[COLUMN_STRING]).equals("string");
  assert(short_column_strings[COLUMN_VARSTRING]).equals("varstring");

  assert(short_column_strings[COLUMN_PHONE]).equals("phone");
  assert(short_column_strings[COLUMN_DATE]).equals("date");
  assert(short_column_strings[COLUMN_CURRENCY]).equals("currency");
  assert(short_column_strings[COLUMN_ENUM]).equals("enum");
}

}
