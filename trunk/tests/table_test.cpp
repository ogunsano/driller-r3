#include "test.h"
#include "fixture.h"
#include "database.h"

using namespace Driller;

TEST_SUITE(table_tests)

FIXTURE(table_fixture)

void set_up(){
  Database db = Database::from_file("tests/data/database.xml");
  std::vector<Table> tables = db.get_tables();
  table1 = db.table_at(0);
  table2 = db.table_at(1);
}

Table table1, table2;

};

TEST(empty_constructor)
  Table table;

  assert(table.get_name()).equals("");
  assert(table.get_file_name()).equals("");
  assert(table.get_data_offset()).equals(0);
  assert(table.get_row_length()).equals(1);
}

TEST(constructor)
  Table table("Name", "File name", 123, 456);

  assert(table.get_name()).equals("Name");
  assert(table.get_file_name()).equals("File name");
  assert(table.get_data_offset()).equals(123);
  assert(table.get_row_length()).equals(456);
}

FIXTURE_TEST(column_count, table_fixture)
  assert(table1.column_count()).equals(15);
  assert(table2.column_count()).equals(0);
}

FIXTURE_TEST(clear, table_fixture)
  table1.clear();

  assert(table1.get_name()).equals("");
  assert(table1.get_file_name()).equals("");
  assert(table1.get_data_offset()).equals(0);
  assert(table1.get_row_length()).equals(1);

  assert(table1.column_count()).equals(0);
}

FIXTURE_TEST(name, table_fixture)
  assert(table1.get_name()).equals("Table 1");

  table1.set_name("Testing");

  assert(table1.get_name()).equals("Testing");
}

FIXTURE_TEST(file_name, table_fixture)
  assert(table1.get_file_name()).equals("table_1.dat");

  table1.set_name("Testing");

  assert(table1.get_name()).equals("Testing");
}

FIXTURE_TEST(data_offset, table_fixture)
  assert(table2.get_data_offset()).equals(123);

  table2.set_data_offset(987);

  assert(table2.get_data_offset()).equals(987);
}

FIXTURE_TEST(row_length, table_fixture)
  assert(table2.get_row_length()).equals(456);

  table2.set_row_length(987);

  assert(table2.get_row_length()).equals(987);
}

FIXTURE_TEST(column_at, table_fixture)
  Column col = table1.column_at(0);

  assert(col.get_name()).equals("unknown_col");
}

FIXTURE_TEST(add_column, table_fixture)
  assert(table1.column_count()).equals(15);
  table1.add_column(Column());
  assert(table1.column_count()).equals(16);
}

FIXTURE_TEST(remove_column, table_fixture)
  assert(table1.column_count()).equals(15);
  table1.remove_column(0);
  assert(table1.column_count()).equals(14);
  assert(table1.column_at(0).get_name()).equals("bool_col");
}

FIXTURE_TEST(get_columns, table_fixture)
  std::vector<Column> columns = table1.get_columns();

  assert(columns.size()).equals(15);
  assert(columns.at(0).get_name()).equals("unknown_col");
  assert(columns.at(1).get_name()).equals("bool_col");
}

}
