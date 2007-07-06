#include <copper.hpp>
#include "../src/database/database.h"

using namespace Driller;

TEST_SUITE(table_tests) {

FIXTURE(table_fixture) {
  Table table1, table2;

  SET_UP {
    Database db = Database::from_file("tests/data/database.xml");
    std::vector<Table> tables = db.get_tables();
    table1 = db.table_at(0);
    table2 = db.table_at(1);
  }
}

TEST(empty_constructor) {
  Table table;

  ASSERT(equal("", table.get_name()));
  ASSERT(equal("", table.get_file_name()));
  ASSERT(equal(0u, table.get_data_offset()));
  ASSERT(equal(1u, table.get_row_length()));
}

TEST(constructor) {
  Table table("Name", "File name", 123, 456);

  ASSERT(equal("Name", table.get_name()));
  ASSERT(equal("File name", table.get_file_name()));
  ASSERT(equal(123u, table.get_data_offset()));
  ASSERT(equal(456u, table.get_row_length()));
}

FIXTURE_TEST(column_count, table_fixture) {
  ASSERT(equal(15u, table1.column_count()));
  ASSERT(equal(0u, table2.column_count()));
}

FIXTURE_TEST(clear, table_fixture) {
  table1.clear();

  ASSERT(equal("", table1.get_name()));
  ASSERT(equal("", table1.get_file_name()));
  ASSERT(equal(0u, table1.get_data_offset()));
  ASSERT(equal(1u, table1.get_row_length()));

  ASSERT(equal(0u, table1.column_count()));
}

FIXTURE_TEST(name, table_fixture) {
  ASSERT(equal("Table 1", table1.get_name()));

  table1.set_name("Testing");

  ASSERT(equal("Testing", table1.get_name()));
}

FIXTURE_TEST(file_name, table_fixture) {
  ASSERT(equal("table_1.dat", table1.get_file_name()));

  table1.set_name("Testing");

  ASSERT(equal("Testing", table1.get_name()));
}

FIXTURE_TEST(data_offset, table_fixture) {
  ASSERT(equal(123u, table2.get_data_offset()));

  table2.set_data_offset(987);

  ASSERT(equal(987u, table2.get_data_offset()));
}

FIXTURE_TEST(row_length, table_fixture) {
  ASSERT(equal(456u, table2.get_row_length()));

  table2.set_row_length(987);

  ASSERT(equal(987u, table2.get_row_length()));
}

FIXTURE_TEST(column_at, table_fixture) {
  Column col = table1.column_at(0);
  ASSERT(equal("unknown_col", col.get_name()));

  // Const version
  const Table& const_table1 = table1;
  Column col2 = const_table1.column_at(0);
  ASSERT(equal("unknown_col", col2.get_name()));
}

FIXTURE_TEST(add_column, table_fixture) {
  ASSERT(equal(15u, table1.column_count()));
  table1.add_column(Column());
  ASSERT(equal(16u, table1.column_count()));
}

FIXTURE_TEST(remove_column, table_fixture) {
  ASSERT(equal(15u, table1.column_count()));
  table1.remove_column(0);
  ASSERT(equal(14u, table1.column_count()));
  ASSERT(equal("bool_col", table1.column_at(0).get_name()));
}

FIXTURE_TEST(get_columns, table_fixture) {
  std::vector<Column> columns = table1.get_columns();

  ASSERT(equal(15u, columns.size()));
  ASSERT(equal("unknown_col", columns.at(0).get_name()));
  ASSERT(equal("bool_col", columns.at(1).get_name()));
}

}
