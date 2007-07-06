#include <copper.hpp>
#include "../src/database/database.h"

using namespace Driller;

TEST_SUITE(database_tests) {

FIXTURE(db_fixture) {
  Database db;

  SET_UP {
    db = Database::from_file("tests/data/database.xml");
  }
}

TEST(empty_constructor) {
  Database db;
  ASSERT(equal("", db.get_name()));
}

TEST(constructor) {
  Database db("Test name");
  ASSERT(equal("Test name", db.get_name()));
}

FIXTURE_TEST(table_count, db_fixture) {
  ASSERT(equal(2u, db.table_count()));
}

FIXTURE_TEST(clear, db_fixture) {
  db.clear();

  ASSERT(equal("", db.get_name()));
  ASSERT(equal(0u, db.table_count()));
}

TEST(name) {
  Database db("Testing 1");
  ASSERT(equal("Testing 1", db.get_name()));

  db.set_name("Testing 2");

  ASSERT(equal("Testing 2", db.get_name()));
}

FIXTURE_TEST(table_at, db_fixture) {
  ASSERT(equal("Table 1", db.table_at(0).get_name()));
  ASSERT(equal("Table 2", db.table_at(1).get_name()));

  // Const versions
  const Database& cdb = db;
  ASSERT(equal("Table 1", cdb.table_at(0).get_name()));
  ASSERT(equal("Table 2", cdb.table_at(1).get_name()));
}

FIXTURE_TEST(add_table, db_fixture) {
  db.add_table(Table());
  ASSERT(equal(3u, db.table_count()));
  ASSERT(equal("", db.table_at(0).get_name()));
}

FIXTURE_TEST(remove_table, db_fixture) {
  db.remove_table(0);
  ASSERT(equal(1u, db.table_count()));
  ASSERT(equal("Table 2", db.table_at(0).get_name()));
}

TEST(data_path) {
  Database::set_data_path("test data path");
  ASSERT(equal("test data path", Database::get_data_path()));
}

FIXTURE_TEST(get_tables, db_fixture) {
  std::vector<Table> tables = db.get_tables();

  ASSERT(equal(2u, tables.size()));
  ASSERT(equal("Table 1", tables.at(0).get_name()));
  ASSERT(equal("Table 2", tables.at(1).get_name()));
}

}
