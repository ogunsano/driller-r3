#include "test.h"
#include "fixture.h"
#include "database/column.h"

using namespace Driller;

TEST_SUITE(column_tests)

FIXTURE(column_fixture)

  void set_up(){
    col = Column("", COLUMN_UNKNOWN);
  }

  Column col;
};

TEST(empty_constructor)
  Column col;

  assert(col.get_name()).equals("");
  assert(col.get_type()).equals(COLUMN_UNKNOWN);
  assert(col.get_offset()).equals(0);
  assert(col.get_length()).equals(0);
  assert(col.get_indexed()).is_false();
}

TEST(constructor)
  Column col("name", COLUMN_NUM_TYPES, 5, 6, false);

  assert(col.get_name()).equals("name");
  assert(col.get_type()).equals(COLUMN_UNKNOWN); // invalid type -> COLUMN_UNKNOWN
  assert(col.get_offset()).equals(5);
  assert(col.get_length()).equals(0); // should be 0 if type doesn't use length
  assert(col.get_indexed()).is_false();
}

FIXTURE_TEST(type, column_fixture)
  assert(col.get_type()).equals(COLUMN_UNKNOWN);
  col.set_type(COLUMN_BOOL);
  assert(col.get_type()).equals(COLUMN_BOOL);
}

FIXTURE_TEST(name, column_fixture)
  assert(col.get_name()).equals("");
  col.set_name("test");
  assert(col.get_name()).equals("test");
}

FIXTURE_TEST(offset, column_fixture)
  assert(col.get_offset()).equals(0);
  col.set_offset(10);
  assert(col.get_offset()).equals(10);
}

FIXTURE_TEST(length, column_fixture)
  assert(col.get_length()).equals(0);
  col.set_length(10);
  assert(col.get_length()).equals(10);
}

FIXTURE_TEST(indexed, column_fixture)
  assert(col.get_indexed()).is_false();
  col.set_indexed(true);
  assert(col.get_indexed()).is_true();
}

TEST(get_int8)
  uint8 data[] = {150, 175, 200, 225};
  assert(Column::get_int8(data)).equals(-106);
}

TEST(get_uint8)
  uint8 data[] = {150, 175, 200, 225};
  assert(Column::get_uint8(data)).equals(150);
}

TEST(get_int16)
  uint8 data[] = {150, 175, 200, 225};
  assert(Column::get_int16(data)).equals(-20586);
}

TEST(get_uint16)
  uint8 data[] = {150, 175, 200, 225};
  assert(Column::get_uint16(data)).equals(44950);
}

TEST(get_int32)
  uint8 data[] = {150, 175, 200, 225};
  assert(Column::get_int32(data)).equals(-506941546);
}

TEST(get_uint32)
  uint8 data[] = {150, 175, 200, 225};
  // use hex, decimal is too long
  assert(Column::get_uint32(data)).equals(0xE1C8AF96);
}

TEST(extract_unknown)
  Column col;
  assert(col.extract_data(NULL)).equals("unknown");
}

TEST(extract_bool)
  Column col("", COLUMN_BOOL);
  uint8 true_data = 1;
  uint8 false_data = 0;

  assert(col.extract_data(&true_data)).equals("True");
  assert(col.extract_data(&false_data)).equals("False");
}

TEST(extract_int8)
  Column col("", COLUMN_INT8);
  uint8 data[] = {150, 175, 200, 225};
  assert(col.extract_data(data)).equals("-106");
}

TEST(extract_uint8)
  Column col("", COLUMN_UINT8);
  uint8 data[] = {150, 175, 200, 225};
  assert(col.extract_data(data)).equals("150");
}

TEST(extract_int16)
  Column col("", COLUMN_INT16);
  uint8 data[] = {150, 175, 200, 225};
  assert(col.extract_data(data)).equals("-20586");
}

TEST(extract_uint16)
  Column col("", COLUMN_UINT16);
  uint8 data[] = {150, 175, 200, 225};
  assert(col.extract_data(data)).equals("44950");
}

TEST(extract_int32)
  Column col("", COLUMN_INT32);
  uint8 data[] = {150, 175, 200, 225};
  assert(col.extract_data(data)).equals("-506941546");
}

TEST(extract_uint32)
  Column col("", COLUMN_UINT32);
  uint8 data[] = {150, 175, 200, 225};
  assert(col.extract_data(data)).equals("3788025750");
}

TEST(extract_blob)
  Column col("", COLUMN_BLOB, 0, 4);
  uint8 data[] = {0x96, 0xAF, 0xC8, 0xE1};
  assert(col.extract_data(data)).equals("96 AF C8 E1");
}

TEST(extract_string)
  Column col("", COLUMN_STRING, 0, 20);
  uint8 data[] = {'T', 'e', 's', 't', 'i', 'n', 'g', '!', '!', 0};
  assert(col.extract_data(data)).equals("Testing!!");

  col.set_length(7);
  assert(col.extract_data(data)).equals("Testing");
}

TEST(extract_varstring)
  Column col("", COLUMN_VARSTRING, 6);
  uint8 data[] = {0, 0, 15, 0, 0, 0, 'T', 'e', 's', 't', 'i', 'n', 'g', '!', '!', 0};

  assert(col.extract_data(data)).equals("Testing!!");
}

TEST(extract_phone)
  Column col("", COLUMN_PHONE, 0);
  uint8 data[] = {'1', '2', '3', '4', '5', '6', '7', 0};

  assert(col.extract_data(data)).equals("123-4567");
}

TEST(extract_long_phone)
  Column col("", COLUMN_PHONE, 0);
  uint8 data[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 0};
  assert(col.extract_data(data)).equals("123-456-7890");
}

TEST(extract_date)
  Column col("", COLUMN_DATE, 0);
  uint8 data[] = {0xCB, 0xAB, 0x01, 0x00}; // 109515, AKA January 2 2000

  assert(col.extract_data(data)).equals("2000-1-2");
}

TEST(extract_currency)
  Column col("", COLUMN_CURRENCY, 0);
  uint8 data[] = {0xD4, 0x30, 0x00, 0x00}; // 12500, AKA 125.00

  assert(col.extract_data(data)).equals("125.00");
}

TEST(extract_enum)
  Column col("", COLUMN_ENUM, 0);
  col.enumeration.add_case("First");
  col.enumeration.add_case("Second");
  col.enumeration.add_case("Third");

  uint8 first = 0, second = 1, third = 2;

  assert(col.extract_data(&first)).equals("First");
  assert(col.extract_data(&second)).equals("Second");
  assert(col.extract_data(&third)).equals("Third");
}

}
