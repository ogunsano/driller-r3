/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * database.cpp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <sstream>
#include <algorithm>

#include <libxml/xmlwriter.h>

#include "database.h"
#include "misc.h"

/** Perform one-time libXML initialization */
void init_libxml() throw () {
  static bool initialized = false;
  if (!initialized){
    LIBXML_TEST_VERSION
    initialized = true;
  }
}

/** Automatically throw an exception if a libXML call fails */
void wrap_xml(const int return_code) throw (Errors::FileParseError) {
  if (return_code < 0){
    throw Errors::FileParseError("");
  }
}

/** Write any kind of attribute */
template <class T>
void write_attribute(
  xmlTextWriter* writer,
  const std::string& attribute_name,
  const T& value){

  // Used for formatting
  static std::stringstream ss;

  ss << value;
  wrap_xml(xmlTextWriterWriteAttribute(writer,
    BAD_CAST(attribute_name.c_str()),
    BAD_CAST(ss.str().c_str())
  ));
  ss.str("");
  ss.clear();
}

/** Write boolean attribute */
template <>
void write_attribute<bool>(
  xmlTextWriter* writer,
  const std::string& attribute_name,
  const bool& value){

  wrap_xml(xmlTextWriterWriteAttribute(writer,
    BAD_CAST(attribute_name.c_str()),
    BAD_CAST(value? "true" : "false")
  ));
}

namespace Driller {

std::string Database::data_path = ".";

Database::Database(const std::string& _name) throw ():
  name(_name){}

Database Database::from_file(const std::string& file) throw (
  Errors::DuplicateEnumID,
  Errors::FileParseError,
  Errors::FileReadError) {

  Database db;
  db.load(file);
  return db;
}

Database Database::from_buffer(const char* buffer)
  throw (Errors::DuplicateEnumID, Errors::FileParseError) {

  Database db;
  db.load_from_buffer(buffer);
  return db;
}

Database::~Database() throw () {
  clear();
}

/**
  Parse a libXML document into a database

  @param db The database to parse into
  @param xml The libXML document to parse
  @param filename If the XML document being parsed is a file, this should
  contain the file's name
*/
void load_from_libxml(Database* db, xmlDoc* xml,
  const std::string& filename = "") throw (
  Errors::DuplicateEnumID,
  Errors::FileParseError) {

  // Clear existing tables
  db->clear();

  // <database> element
  xmlNode* db_element = xmlDocGetRootElement(xml);

  // Get the database's name
  xmlChar* raw_name = xmlGetProp(db_element, BAD_CAST("name"));

  if (!raw_name){
    throw Errors::MissingAttributeError(filename, "database", "name");
  }

  db->set_name(reinterpret_cast<char*>(raw_name));
  xmlFree(raw_name);

  // Parse each table
  for (xmlNode* ii = db_element->children; ii; ii = ii->next){
    if (ii->type == XML_ELEMENT_NODE){
      db->add_table(Table(ii));
    }
  }

  // Free the document
  xmlFreeDoc(xml);
  xmlCleanupParser();
}

void Database::load(const std::string& file) throw (
  Errors::DuplicateEnumID,
  Errors::FileParseError,
  Errors::FileReadError) {

  // Initialize libXML
  init_libxml();

  // Use libXML to parse the file
  xmlDoc* db_xml = xmlReadFile(file.c_str(), NULL, 0);

  if (!db_xml){
    throw Errors::FileParseError(file);
  }

  try {
    load_from_libxml(this, db_xml);
  }
  catch (...) {
    xmlFreeDoc(db_xml);
    throw;
  }
}

void Database::load_from_buffer(const char* buffer) throw (
  Errors::DuplicateEnumID,
  Errors::FileParseError) {

  // Initialize libXML
  init_libxml();

  // Use libXML to parse the file
  xmlDoc* db_xml = xmlReadMemory(buffer, static_cast<int>(strlen(buffer)),
    "", NULL, 0);

  if (!db_xml){
    throw Errors::FileParseError("in-memory buffer");
  }

  try {
    load_from_libxml(this, db_xml);
  }
  catch (...) {
    xmlFreeDoc(db_xml);
    throw;
  }
}

unsigned int Database::table_count() const throw () {
  return static_cast<unsigned int>(tables.size());
}

void Database::clear() throw () {
  tables.clear();
  set_name("");
}

void Database::set_name(const std::string& _name) throw () {
  name = _name;
}

std::string Database::get_name() const throw () {
  return name;
}

std::vector<Table> Database::get_tables() const throw () {
  return tables;
}

Table& Database::table_at(const unsigned int index) throw () {
  return tables.at(index);
}

const Table& Database::table_at(const unsigned int index) const throw () {
  return tables.at(index);
}

// Used in add_table
static bool table_cmp(const Table& a, const Table& b) {
   return a.get_name() < b.get_name();
}

void Database::add_table(const Table& table) throw () {
  tables.push_back(table);

  // Probably slow to sort on each add, but better than duplicating code. Plus,
  // this won't get called often enough to cause performance problems
  std::sort(tables.begin(), tables.end(), table_cmp);
}

void Database::remove_table(const unsigned int index) throw () {
  if (index < table_count()){
    std::vector<Table>::iterator iter = tables.begin() + index;
    tables.erase(iter);
  }
}

void Database::set_data_path(const std::string& path) throw () {
  data_path = path;
}

std::string Database::get_data_path() throw () {
  return data_path;
}

void write_db_to(xmlTextWriter* writer, const Database& db)
  throw (Errors::FileParseError) {

  // Set indenting to two spaces
  wrap_xml(xmlTextWriterSetIndent(writer, 1));
  wrap_xml(xmlTextWriterSetIndentString(writer, BAD_CAST("  ")));

  // Start the document with the 'database' element
  wrap_xml(xmlTextWriterStartDocument(writer, "1.0", "utf-8", NULL));
  wrap_xml(xmlTextWriterStartElement(writer, BAD_CAST("database")));

  write_attribute(writer, "name", db.get_name());

  // For each table in the database
  std::vector<Table> tables = db.get_tables();
  std::vector<Table>::const_iterator table;
  for (table = tables.begin(); table != tables.end(); table++){

    wrap_xml(xmlTextWriterStartElement(writer, BAD_CAST("table")));

    // Write the table's attributes
    write_attribute(writer, "name", table->get_name());
    write_attribute(writer, "file", table->get_file_name());
    write_attribute(writer, "data_offset", table->get_data_offset());
    write_attribute(writer, "row_length", table->get_row_length());

    // For each column in the table
    std::vector<Column> columns = table->get_columns();
    std::vector<Column>::const_iterator column;
    for (column = columns.begin(); column != columns.end(); column++){

      // Start the column element
      wrap_xml(xmlTextWriterStartElement(writer,
        BAD_CAST(
          short_column_strings[column->get_type()].c_str()
        )
      ));

      // Write attributes column to every column
      write_attribute(writer, "name", column->get_name());
      write_attribute(writer, "offset", column->get_offset());

      // If the column needs a length attribute, write it
      if (column->needs_length()){
        write_attribute(writer, "length", column->get_length());
      }

      // Only specify the index state if it is true
      if (column->get_indexed()){
       write_attribute(writer, "indexed", true);
      }

      // If the column is an enumeration, output the enumeration cases
      if (column->get_type() == COLUMN_ENUM){
        std::list<EnumCase> case_list = column->enumeration.get_case_list();
        std::list<EnumCase>::const_iterator enum_case;

        for (enum_case = case_list.begin();
          enum_case != case_list.end(); enum_case++){

          wrap_xml(xmlTextWriterStartElement(writer, BAD_CAST("case")));

          write_attribute(writer, "id", enum_case->id);
          write_attribute(writer, "value", enum_case->value);

          wrap_xml(xmlTextWriterEndElement(writer));
        }
      }

      // Close the column element
      wrap_xml(xmlTextWriterEndElement(writer));
    }

    wrap_xml(xmlTextWriterEndElement(writer));
  }

  wrap_xml(xmlTextWriterEndElement(writer));

  wrap_xml(xmlTextWriterEndDocument(writer));
}

std::ostream& operator<<(std::ostream& out, const Database& db)
  throw (Errors::FileParseError) {

  init_libxml();

  xmlBuffer* buf = xmlBufferCreate();

  if (!buf){
    throw Errors::FileParseError("");
  }

  xmlTextWriter* writer = xmlNewTextWriterMemory(buf, 0);

  if (!writer){
    xmlBufferFree(buf);
    throw Errors::FileParseError("");
  }

  try {
    write_db_to(writer, db);
  }

  catch (...) {
    xmlFreeTextWriter(writer);
    xmlBufferFree(buf);
    throw;
  }

  xmlFreeTextWriter(writer);
  out << buf->content;
  xmlBufferFree(buf);

  return out;
}

} // namespace

