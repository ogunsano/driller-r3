/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * mysql_sink.cpp
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

#include "mysql_sink.h"
#include <sstream>

#define NO_CLIENT_LONG_LONG
#include <mysql.h>

namespace Errors {

MySQLError::MySQLError(MYSQL* connection) throw():
  message(mysql_error(connection)){}

MySQLError::~MySQLError() throw(){}

std::string MySQLError::error_message() const throw(){
  return message;
}

} // namespace

namespace Driller {

////////////////////////////
// Misc utility functions //
////////////////////////////

/**
  Use this function to get a buffer large enough to hold @string

  @param string The string that must be held by the buffer

  @return A pointer to the string. Do not free or delete this. This pointer
  will be overwritten every time this function is called
*/
char* get_escaped_string_buffer(const std::string& string) throw(){
  static char* buffer = NULL;
  static int buffer_len = 0;

  int string_len = string.size();
  if ((2 * string_len) + 1 > buffer_len){
    buffer_len = (2 * string_len) + 1;
    delete [] buffer;
    buffer = new char[buffer_len];
  }

  return buffer;
}

MySQLSink::MySQLSink(
  const std::string& host,
  const std::string& username,
  const std::string& password,
  const std::string& database) throw (Errors::MySQLError){

  connection = mysql_init(NULL);
  mysql_options(connection, MYSQL_READ_DEFAULT_GROUP, "driller");

  // FIXME: port port in an argument
  if(!mysql_real_connect(connection, host.c_str(), username.c_str(),
    password.c_str(), database.c_str(), 3306, NULL, 0)){

    throw Errors::MySQLError(connection);
  }
}

MySQLSink::~MySQLSink() throw(){
}

MySQLSink& MySQLSink::operator<<(const Database& db)
  throw (Errors::MySQLError){

  std::vector<Table> tables = db.get_tables();
  std::vector<Table>::const_iterator table;
  for (table = tables.begin(); table != tables.end(); table++){
    (*this) << (*table);
  }

  return (*this);
}

MySQLSink& MySQLSink::operator<<(const Table& table)
  throw (Errors::MySQLError){

  // Delete the old table, if it exists
  std::string table_name = make_safe_name(table.get_name());
  send_query("DROP TABLE IF EXISTS " + table_name);

  // Create the table
  std::string buffer;
  buffer = "CREATE TABLE " + table_name + " (";

  std::vector<Column> column_list = table.get_columns();
  std::vector<Column>::iterator column;
  for (column = column_list.begin();
    column != column_list.end();
    column++){

    std::string type = sql_type_from_column(*column);
    std::string name = make_safe_name(column->get_name());

    buffer += "`" + name + "` " + type + ", ";

    if (column->get_indexed()){
      if (!(column->get_type() == COLUMN_BLOB ||
        column->get_type() == COLUMN_VARSTRING ||
        (column->get_type() == COLUMN_STRING && column->get_length() > 255))){

          buffer += " INDEX(`" + name + "`), ";
      }
    }
  }

  buffer.erase(buffer.size() - 2, 2);
  buffer += ")";

  send_query(buffer);

  // Lock the table, and disable keys
  send_query("LOCK TABLES " + table_name + " WRITE");
  send_query("ALTER TABLE " + table_name + " DISABLE KEYS");

  buffer = "INSERT INTO " + table_name + " VALUES ";

  const ResultSet* result = table.extract_data();

  try {

    unsigned int i = 0;
    for (unsigned int row = 0; row < result->row_count(); row++){
      const char** data = (*result)[row];
      buffer += "(";
      for (unsigned int col = 0; col < result->column_count(); col++){
        buffer += '"';
        buffer += make_safe_string(data[col]);
        buffer += "\",";
      }


      /* MySQL limits the size of queries. To be safe, make a new query every
         5000 rows */
      // FIXME: should do some sort of auto-detection
      if (i == 5000){
        buffer.replace(buffer.size() - 1, 1, ")");
        send_query(buffer);

        i = 0;
        buffer = "INSERT INTO " + table_name + " VALUES ";
      }

      else {
        buffer.replace(buffer.size() - 1, 2, "),");
        ++i;
      }
    }


    // Send the query
    buffer.erase(buffer.size() - 1, 1);
    send_query(buffer);
    }

  catch (const Errors::MySQLError& e){
    delete result;
    throw;
  }

  delete result;

  // Un-lock the table, and re-enable keys
  send_query("UNLOCK TABLES");
  send_query("ALTER TABLE " + table_name + " ENABLE KEYS");

  return (*this);
}

const char* MySQLSink::make_safe_string(const std::string& string) const
  throw(){

  char* buffer = get_escaped_string_buffer(string);
  mysql_real_escape_string(connection, buffer, string.c_str(), string.size());
  return buffer;
}

const char* MySQLSink::make_safe_name(std::string name) const throw(){
  // Replace spaces with underscores
  replace(name.begin(), name.end(), ' ', '_');

  return make_safe_string(name);
}

void MySQLSink::send_query(const std::string& query) const
  throw(Errors::MySQLError){

  if (mysql_real_query(connection, query.c_str(), query.size())){
    throw Errors::MySQLError(connection);
  }
}

std::string MySQLSink::sql_type_from_column(const Column& col) const throw(){
  std::string buffer;

  switch(col.get_type()){
    case COLUMN_UNKNOWN:
    case COLUMN_NUM_TYPES:
      // Contents of an unknown column is always "unknown"
      buffer = "CHAR(7)";
    break;

    case COLUMN_BOOL:
      buffer = "TINYINT(1) UNSIGNED";
    break;

    // Integer types
    case COLUMN_INT8:
      buffer = "TINYINT";
    break;

    case COLUMN_UINT8:
      buffer = "TINYINT UNSIGNED";
    break;

    case COLUMN_INT16:
      buffer = "SMALLINT";
    break;

    case COLUMN_UINT16:
      buffer = "SMALLINT UNSIGNED";
    break;

    case COLUMN_INT32:
      buffer = "INT";
    break;

    case COLUMN_UINT32:
      buffer = "INT UNSIGNED";
    break;

    // String types
    case COLUMN_BLOB:
      buffer = "BLOB";
    break;

    case COLUMN_STRING:
      if (col.get_length() < 256){
        std::stringstream ss;
        ss << "VARCHAR(" << col.get_length() << ")";
        buffer = ss.str();
      }
      
      else {
        buffer = "TEXT";
      }
    break;

    case COLUMN_VARSTRING:
      buffer = "TEXT";
    break;


    // Special types
    case COLUMN_PHONE:
      buffer = "VARCHAR(11)";
    break;

    case COLUMN_DATE:
      buffer = "DATE";
    break;

    case COLUMN_CURRENCY:
      buffer = "DECIMAL(10,2)";
    break;

    case COLUMN_ENUM: {
      std::stringstream ss;
      ss << "ENUM (";

      std::list<EnumCase> case_list = col.enumeration.get_case_list();
      std::list<EnumCase>::iterator iter;
      for (iter = case_list.begin();
        iter != case_list.end();
        iter++){

        ss << "'" << make_safe_string((*iter).value) << "', ";
      }

      buffer = ss.str();
      buffer.erase(buffer.size() - 2, 2);

      buffer += ")";
    break;
    }
  }

  return buffer;
}

} // namespace
