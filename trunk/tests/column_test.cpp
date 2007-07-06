#include <copper.hpp>
#include "../src/database/column.h"

using namespace Driller;

TEST_SUITE(column_tests) {

FIXTURE(column_fixture) {
  Column col;

  SET_UP {
    col = Column("", COLUMN_UNKNOWN);
  }
}

TEST(empty_constructor) {
  Column col;

  ASSERT(equal("", col.get_name()));
  ASSERT(equal(COLUMN_UNKNOWN, col.get_type()));
  ASSERT(equal(0u, col.get_offset()));
  ASSERT(equal(0u, col.get_length()));
  ASSERT(!col.get_indexed());
}

TEST(constructor) {
  Column col("name", COLUMN_NUM_TYPES, 5, 6, false);

  ASSERT(equal("name", col.get_name()));
  // invalid type -> COLUMN_UNKNOWN
  ASSERT(equal(COLUMN_UNKNOWN, col.get_type()));
  ASSERT(equal(5u, col.get_offset()));
  // should be 0 if type doesn't use length
  ASSERT(equal(0u, col.get_length()));
  ASSERT(!col.get_indexed());
}

FIXTURE_TEST(type, column_fixture) {
  ASSERT(equal(COLUMN_UNKNOWN, col.get_type()));
  col.set_type(COLUMN_BOOL);
  ASSERT(equal(COLUMN_BOOL, col.get_type()));
  col.set_type(COLUMN_NUM_TYPES);
  ASSERT(equal(COLUMN_UNKNOWN, col.get_type()));
}

FIXTURE_TEST(name, column_fixture) {
  ASSERT(equal("", col.get_name()));
  col.set_name("test");
  ASSERT(equal("test", col.get_name()));
}

FIXTURE_TEST(offset, column_fixture) {
  ASSERT(equal(0u, col.get_offset()));
  col.set_offset(10);
  ASSERT(equal(10u, col.get_offset()));
}

FIXTURE_TEST(length, column_fixture) {
  ASSERT(equal(0u, col.get_length()));
  col.set_length(10);
  ASSERT(equal(10u, col.get_length()));
}

FIXTURE_TEST(indexed, column_fixture) {
  ASSERT(!col.get_indexed());
  col.set_indexed(true);
  ASSERT(col.get_indexed());
}

TEST(needs_length) {
  for (unsigned int ii = COLUMN_UNKNOWN; ii < COLUMN_NUM_TYPES; ii++) {
    Column col("", static_cast<ColumnType>(ii));
    if (ii == COLUMN_STRING || ii == COLUMN_BLOB) {
      ASSERT(col.needs_length());
    }

    else {
      ASSERT(!col.needs_length());
    }
  }
}

TEST(get_int8) {
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal(-106, Column::get_int8(data)));
}

TEST(get_uint8) {
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal(150u, Column::get_uint8(data)));
}

TEST(get_int16) {
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal(-20586, Column::get_int16(data)));
}

TEST(get_uint16) {
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal(44950u, Column::get_uint16(data)));
}

TEST(get_int32) {
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal(-506941546, Column::get_int32(data)));
}

TEST(get_uint32) {
  uint8 data[] = {150, 175, 200, 225};
  // use hex, decimal is too long
  ASSERT(equal(0xE1C8AF96, Column::get_uint32(data)));
}

FIXTURE(extraction_fixture) {
  char* buffer;
  unsigned int buffer_size = 30;

  SET_UP {
    buffer = new char[buffer_size];
  }

  TEAR_DOWN {
    delete [] buffer;
  }
}

FIXTURE_TEST(extract_unknown, extraction_fixture) {
  Column col;
  ASSERT(equal("unknown", col.extract_data(NULL, buffer, buffer_size)));
}

FIXTURE_TEST(extract_bool, extraction_fixture) {
  Column col("", COLUMN_BOOL);
  uint8 true_data = 1;
  uint8 false_data = 0;

  ASSERT(equal("True", col.extract_data(&true_data, buffer, buffer_size)));
  ASSERT(equal("False", col.extract_data(&false_data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_int8, extraction_fixture) {
  Column col("", COLUMN_INT8);
  uint8 positive_data[] = {100, 175, 200, 225};
  uint8 negative_data[] = {150, 175, 200, 225};
  ASSERT(equal("100", col.extract_data(positive_data, buffer, buffer_size)));
  ASSERT(equal("-106", col.extract_data(negative_data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_uint8, extraction_fixture) {
  Column col("", COLUMN_UINT8);
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal("150", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_int16, extraction_fixture) {
  Column col("", COLUMN_INT16);
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal("-20586", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_uint16, extraction_fixture) {
  Column col("", COLUMN_UINT16);
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal("44950", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_int32, extraction_fixture) {
  Column col("", COLUMN_INT32);
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal("-506941546", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_uint32, extraction_fixture) {
  Column col("", COLUMN_UINT32);
  uint8 data[] = {150, 175, 200, 225};
  ASSERT(equal("3788025750", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_blob, extraction_fixture) {
  Column col("", COLUMN_BLOB, 0, 4);
  uint8 data[] = {0x96, 0xAF, 0xC8, 0xE1};
  ASSERT(equal("96 AF C8 E1", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_string, extraction_fixture) {
  Column col("", COLUMN_STRING, 0, 20);
  uint8 data[] = {'T', 'e', 's', 't', 'i', 'n', 'g', '!', '!', 0};
  ASSERT(equal("Testing!!", col.extract_data(data, buffer, buffer_size)));

  col.set_length(7);
  ASSERT(equal("Testing", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_varstring, extraction_fixture) {
  Column col("", COLUMN_VARSTRING, 6);
  uint8 data[] = {0, 0, 15, 0, 0, 0, 'T', 'e', 's', 't', 'i', 'n', 'g', '!', '!', 0};

  ASSERT(equal("Testing!!", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_phone, extraction_fixture) {
  Column col("", COLUMN_PHONE, 0);
  uint8 data[] = {'1', '2', '3', '4', '5', '6', '7', 0};

  ASSERT(equal("123-4567", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_long_phone, extraction_fixture) {
  Column col("", COLUMN_PHONE, 0);
  uint8 data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0};
  ASSERT(equal("123-456-7890", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_date, extraction_fixture) {
  Column col("", COLUMN_DATE, 0);
  uint8 data[] = {0xCB, 0xAB, 0x01, 0x00}; // 109515, AKA January 2 2000

  ASSERT(equal("2000-1-2", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_currency, extraction_fixture) {
  Column col("", COLUMN_CURRENCY, 0);
  uint8 data[] = {0xD4, 0x30, 0x00, 0x00}; // 12500, AKA 125.00

  ASSERT(equal("125.00", col.extract_data(data, buffer, buffer_size)));
}

FIXTURE_TEST(extract_enum, extraction_fixture) {
  Column col("", COLUMN_ENUM, 0);
  col.enumeration.add_case("First");
  col.enumeration.add_case("Second");
  col.enumeration.add_case("Third");

  uint8 first = 0, second = 1, third = 2;

  ASSERT(equal("First", col.extract_data(&first, buffer, buffer_size)));
  ASSERT(equal("Second", col.extract_data(&second, buffer, buffer_size)));
  ASSERT(equal("Third", col.extract_data(&third, buffer, buffer_size)));
}

}
