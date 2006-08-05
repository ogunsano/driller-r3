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
#include "database.h"
#include "database/misc.h"

namespace Driller {

void init_libxml() throw () {
  static bool initialized = false;
  if (!initialized){
    LIBXML_TEST_VERSION
    initialized = true;
  }
}

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

void Database::load(const std::string& file) throw (
  Errors::DuplicateEnumID,
  Errors::FileParseError,
  Errors::FileReadError) {

  // Open the file
  FILE* in = fopen(file.c_str(), "r");

  if (!in){
    throw Errors::FileReadError(file);
  }

  // Get the file length
  fseek(in, 0, SEEK_END);
  unsigned int file_size = ftell(in);
  fseek(in, 0, SEEK_SET);

  // Read the file in to a buffer
  char* buffer = new char[file_size + 1];
  fread(buffer, 1, file_size, in);
  buffer[file_size] = 0;

  // Parse the buffer
  load_from_buffer(buffer);

  // Clean up
  delete[] buffer;
  fclose(in);
}

void Database::load_from_buffer(const char* buffer) throw (
  Errors::DuplicateEnumID,
  Errors::FileParseError) {

  // Clear existing tables
  clear();

  // Initialize libXML
  init_libxml();

  // Use libXML to parse the file
  xmlDoc* db_xml = xmlReadMemory(buffer, strlen(buffer), "", NULL, 0);

  if (!db_xml){
    throw Errors::FileParseError("");
  }

  // <database> element
  xmlNode* db_element = xmlDocGetRootElement(db_xml);

  // Get the database's name
  xmlChar* raw_name = xmlGetProp(db_element,
    reinterpret_cast<const xmlChar*>("name"));

  if (!raw_name){
    throw Errors::MissingAttributeError("", "database", "name");
  }

  set_name(reinterpret_cast<char*>(raw_name));
  xmlFree(raw_name);

  // Parse each table
  for (xmlNode* ii = db_element->children; ii; ii = ii->next){
    if (ii->type == XML_ELEMENT_NODE){
      tables.push_back(Table(ii));
    }
  }

  // Free the document
  xmlFreeDoc(db_xml);
  xmlCleanupParser();
}

unsigned int Database::table_count() const throw () {
  return tables.size();
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

void Database::add_table(const Table& table) throw () {
  tables.push_back(table);
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

} // namespace
