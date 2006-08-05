#include "test.h"
#include "fixture.h"
#include "database.h"

using namespace Driller;

TEST_SUITE(database_tests)

FIXTURE(db_fixture)

void set_up(){
  db = Database::from_file("tests/data/database.xml");
}

Database db;

};

TEST(empty_constructor)
  Database db;
  assert(db.get_name()).equals("");
}

TEST(constructor)
  Database db("Test name");
  assert(db.get_name()).equals("Test name");
}

TEST(from_file)
  Database db = Database::from_file("tests/data/database.xml");

  assert(db.get_name()).equals("From a file < > \" & ");
  assert(db.table_count()).equals(2);
}

TEST(load)
  Database db;
  db.load("tests/data/database.xml");

  assert(db.get_name()).equals("From a file < > \" & ");
  assert(db.table_count()).equals(2);
}

FIXTURE_TEST(table_count, db_fixture)
  assert(db.table_count()).equals(2);
}

FIXTURE_TEST(clear, db_fixture)
  db.clear();

  assert(db.get_name()).equals("");
  assert(db.table_count()).equals(0);
}

TEST(name)
  Database db("Testing 1");
  assert(db.get_name()).equals("Testing 1");

  db.set_name("Testing 2");

  assert(db.get_name()).equals("Testing 2");
}

FIXTURE_TEST(table_at, db_fixture)
  assert(db.table_at(0).get_name()).equals("Table 1");
  assert(db.table_at(1).get_name()).equals("Table 2");
}

FIXTURE_TEST(add_table, db_fixture)
  db.add_table(Table());
  assert(db.table_count()).equals(3);
  assert(db.table_at(2).get_name()).equals("");
}

FIXTURE_TEST(remove_table, db_fixture)
  db.remove_table(0);
  assert(db.table_count()).equals(1);
  assert(db.table_at(0).get_name()).equals("Table 2");
}

TEST(data_path)
  Database::set_data_path("test data path");
  assert(Database::get_data_path()).equals("test data path");
}

FIXTURE_TEST(get_tables, db_fixture)
  std::vector<Table> tables = db.get_tables();

  assert(tables.size()).equals(2);
  assert(tables.at(0).get_name()).equals("Table 1");
  assert(tables.at(1).get_name()).equals("Table 2");
}

FIXTURE_TEST(output, db_fixture)
  // Find the expected result of the output
  FILE* in = fopen("tests/data/database.xml", "r");

  if (!in){
    throw Errors::FileReadError("tests/data/database.xml");
  }

  fseek(in, 0, SEEK_END);
  unsigned int file_size = ftell(in);
  fseek(in, 0, SEEK_SET);

  // Read the file in to a buffer
  char* buffer = new char[file_size + 1];
  fread(buffer, 1, file_size, in);
  buffer[file_size] = 0;

  std::string expected = buffer;
  delete[] buffer;

  fclose(in);

  std::stringstream ss;
  ss << db;
  assert(ss.str()).equals(expected);
}

}
