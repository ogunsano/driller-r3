#include <copper.hpp>
#include "../src/database/database.h"

using namespace Driller;

TEST_SUITE(serialization_tests) {

TEST(from_file) {
  Database db = Database::from_file("tests/data/database.xml");

  ASSERT(equal("From a file < > \" & ", db.get_name()));
  ASSERT(equal(2u, db.table_count()));
}

TEST(from_buffer) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"From a buffer\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\"/>"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\"/>"
  "</database>";
  Database db = Database::from_buffer(buffer.c_str());
  ASSERT(equal("From a buffer", db.get_name()));
  ASSERT(equal(2u, db.table_count()));
}

TEST(from_buffer_parse_error) {
  std::string buffer = "Invalid XML file";

  ASSERT(
    throws(
      Errors::FileParseError,
      Database::from_buffer(buffer.c_str())
    )
  );

  ASSERT(
    throws(
      Errors::FileParseError,
      Database::from_buffer(buffer.c_str())
    )
  );
}

TEST(from_file_parse_error) {
  ASSERT(throws(Errors::FileParseError,
                Database::from_file("tests/data/invalid.xml")));
}

TEST(no_database_name) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database/>";

  ASSERT(throws(Errors::MissingAttributeError,
                Database::from_buffer(buffer.c_str())));
}

TEST(no_table_name) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table file=\"\" data_offset=\"0\" row_length=\"1\"/>"
  "</database>";
  ASSERT(throws(Errors::MissingAttributeError,
                Database::from_buffer(buffer.c_str())));
}

TEST(no_table_file) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" data_offset=\"0\" row_length=\"1\"/>"
  "</database>";
  ASSERT(throws(Errors::MissingAttributeError,
                Database::from_buffer(buffer.c_str())));
}

TEST(no_table_data_offset) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" row_length=\"1\"/>"
  "</database>";
  ASSERT(throws(Errors::MissingAttributeError,
                Database::from_buffer(buffer.c_str())));
}

TEST(invalid_table_data_offset) {
  std::string buffer_1 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"invalid\" row_length=\"1\"/>"
  "</database>";

  std::string buffer_2 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"-1\" row_length=\"1\"/>"
  "</database>";
  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_1.c_str())));
  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_2.c_str())));
}

TEST(no_table_row_length) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\"/>"
  "</database>";
  ASSERT(throws(Errors::MissingAttributeError,
                Database::from_buffer(buffer.c_str())));
}

TEST(invalid_table_row_length) {
  std::string buffer_1 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"invalid\"/>"
  "</database>";

  std::string buffer_2 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"-1\"/>"
  "</database>";

  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_1.c_str())));
  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_2.c_str())));
}

TEST(no_column_name) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <unknown offset=\"0\"/>"
  "  </table>"
  "</database>";

  ASSERT(throws(Errors::MissingAttributeError,
                Database::from_buffer(buffer.c_str())));
}

TEST(no_column_offset) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <unknown name=\"\"/>"
  "  </table>"
  "</database>";

  ASSERT(throws(Errors::MissingAttributeError,
                Database::from_buffer(buffer.c_str())));
}

TEST(invalid_column_offset) {
  std::string buffer_1 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <unknown name=\"\" offset=\"invalid\"/>"
  "  </table>"
  "</database>";

  std::string buffer_2 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <unknown name=\"\" offset=\"-1\"/>"
  "  </table>"
  "</database>";

  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_1.c_str())));
  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_2.c_str())));
}

TEST(no_column_length) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <string name=\"\" offset=\"0\"/>"
  "  </table>"
  "</database>";

  ASSERT(throws(Errors::MissingAttributeError,
                Database::from_buffer(buffer.c_str())));
}

TEST(invalid_column_length) {
  std::string buffer_1 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <string name=\"\" offset=\"0\" length=\"invalid\"/>"
  "  </table>"
  "</database>";

  std::string buffer_2 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <string name=\"\" offset=\"0\" length=\"0\"/>"
  "  </table>"
  "</database>";

  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_1.c_str())));
  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_2.c_str())));
}

TEST(invalid_enum_id) {
  std::string buffer_1 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <enum name=\"\" offset=\"0\">"
  "      <case id=\"-1\" value=\"\"/>"
  "    </enum>"
  "  </table>"
  "</database>";

  std::string buffer_2 =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <enum name=\"\" offset=\"0\">"
  "      <case id=\"invalid\" value=\"\"/>"
  "    </enum>"
  "  </table>"
  "</database>";

  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_1.c_str())));
  ASSERT(throws(Errors::InvalidAttributeError,
                Database::from_buffer(buffer_2.c_str())));
}

TEST(duplicate_id) {
  std::string buffer =
  "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
  "<database name=\"\">"
  "  <table name=\"\" file=\"\" data_offset=\"0\" row_length=\"1\">"
  "    <enum name=\"\" offset=\"0\">"
  "      <case id=\"0\" value=\"\"/>"
  "      <case id=\"0\" value=\"\"/>"
  "    </enum>"
  "  </table>"
  "</database>";

  ASSERT(throws(Errors::DuplicateEnumID,
                Database::from_buffer(buffer.c_str())));
}

TEST(load) {
  Database db;
  db.load("tests/data/database.xml");

  ASSERT(equal("From a file < > \" & ", db.get_name()));
  ASSERT(equal(2u, db.table_count()));
}

TEST(output) {
  const std::string filename = "tests/data/database.xml";
  Database db = Database::from_file(filename);

  // Find the expected result of the output
  FILE* in = fopen(filename.c_str(), "r");

  if (!in){
    throw Errors::FileReadError(filename);
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
  ASSERT(equal(expected, ss.str()));
}

}
