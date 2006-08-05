/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * column.cpp
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
#include <errno.h>
#include "database.h"
#include "file_errors.h"
#include "misc.h"

// Windows uses _snprintf instead of snprintf
#ifdef WIN32
  #ifndef snprintf
    #define snprintf _snprintf
  #endif
#endif

namespace Driller {

/**
  Convert an unsigned integer to a string

  @param i The integer to convert
  @param buffer A temporary buffer to hold the string in
  @param buf_size The size of buffer

  @return The start of the new string
*/
char* unsigned_to_string(unsigned int i, char* buffer,
  const unsigned int buf_size) throw () {

  char* buf = buffer + buf_size - 1;
  buf[0] = 0; // null terminator
  char charcode;
  do {
    charcode = (i % 10) + 48;
    i /= 10;
    --buf;
    buf[0] = charcode;
  } while (i != 0);

  return buf;
}

/**
  Convert a signed integer to a string

  @param i The integer to convert
  @param buffer A temporary buffer to hold the string in
  @param buf_size The size of buffer

  @return The start of the new string
*/
char* signed_to_string(int i, char* buffer, unsigned int buf_size) throw () {
  char* buf = buffer + buf_size - 1;
  char charcode;
  buf[0] = 0; // terminator
  if (i < 0){
    do {
      charcode = -((i % 10) - 48);
      i /= 10;
      --buf;
      buf[0] = charcode;
    } while (i != 0);

    --buf;
    buf[0] = '-';
  }

  else {
    do {
      charcode = (i % 10) + 48;
      i /= 10;
      --buf;
      buf[0] = charcode;
    } while (i != 0);
  }

  return buf;
}

Column::Column(
  const std::string& _name,
  const ColumnType _type,
  const unsigned int _offset,
  const unsigned int _length,
  const bool _indexed) throw ():
  type(_type), name(_name), offset(_offset), length(0),
  indexed(_indexed){

  if (type >= COLUMN_NUM_TYPES){
    type = COLUMN_UNKNOWN;
  }

  else if (needs_length()){
    length = _length;
  }
}

Column::Column(xmlNode* node) throw (Errors::FileParseError,
  Errors::DuplicateEnumID):
  length(0){

  const xmlChar* raw_type = node->name;
  xmlChar* raw_name = xmlGetProp(node,
    reinterpret_cast<const xmlChar*>("name"));
  xmlChar* raw_offset = xmlGetProp(node,
    reinterpret_cast<const xmlChar*>("offset"));
  xmlChar* raw_indexed = xmlGetProp(node,
    reinterpret_cast<const xmlChar*>("indexed"));
  xmlChar* raw_length = xmlGetProp(node,
    reinterpret_cast<const xmlChar*>("length"));

  if (!raw_name){
    throw Errors::MissingAttributeError(node->doc->name, "column", "name");
  }

  if (!raw_offset){
    throw Errors::MissingAttributeError(node->doc->name, "column", "offset");
  }

  // Convert raw data into something usable

  // Type of the column
  for (int ii = 0; ii < COLUMN_NUM_TYPES; ii++){
    if (reinterpret_cast<const char*>(raw_type) == short_column_strings[ii]){
      type = static_cast<ColumnType>(ii);
    }
  }

  // Name of the column
  name = reinterpret_cast<char*>(raw_name);
  if (raw_indexed)
    indexed = (std::string(reinterpret_cast<char*>(raw_indexed)) == "true");
  else
    indexed = false;

  std::stringstream strstream;

  strstream << raw_offset;
  strstream >> offset;

  if (needs_length()){
    if (!raw_length){
      throw Errors::MissingAttributeError(node->doc->name, "column", "length");
    }

    strstream.clear();
    strstream << raw_length;
    strstream >> length;
  }

  xmlFree(raw_name);
  xmlFree(raw_offset);
  xmlFree(raw_indexed);
  xmlFree(raw_length);

  // If the column is an enumeration, parse all enumeration values
  if (type == COLUMN_ENUM){
    xmlNode* case_node = node->children;
    xmlChar* raw_id, *raw_value;
    unsigned int id; // not uint8 because the >> operator thinks uint8 is a char
    for (; case_node; case_node = case_node->next){
      if (case_node->type == XML_ELEMENT_NODE){
        // Read the ID and value
        raw_id = xmlGetProp(case_node,
          reinterpret_cast<const xmlChar*>("id"));
        raw_value = xmlGetProp(case_node,
          reinterpret_cast<const xmlChar*>("value"));

        if (!raw_id){
          throw Errors::MissingAttributeError(node->doc->name, "case", "id");
        }

        if (!raw_value){
          throw Errors::MissingAttributeError(node->doc->name, "case", "value");
        }

        strstream.str("");
        strstream.clear();
        strstream << raw_id;
        strstream >> id;

        xmlFree(raw_id);

        enumeration.add_case(id, reinterpret_cast<char*>(raw_value));
        xmlFree(raw_value);
      }
    }
  }
}

void Column::set_type(const ColumnType _type) throw(){
  type = _type;
}

ColumnType Column::get_type() const throw(){
  return type;
}

void Column::set_name(const std::string& _name) throw(){
  name = _name;
}

std::string Column::get_name() const throw(){
  return name;
}

void Column::set_offset(const unsigned int _offset) throw(){
  offset = _offset;
}

unsigned int Column::get_offset() const throw(){
  return offset;
}

void Column::set_length(const unsigned int _length) throw(){
  length = _length;
}

unsigned int Column::get_length() const throw(){
  return length;
}

void Column::set_indexed(const bool _indexed) throw(){
  indexed = _indexed;
}

bool Column::get_indexed() const throw(){
  return indexed;
}

bool Column::needs_length() const throw () {
  return (type == COLUMN_BLOB || type == COLUMN_STRING);
}

const char* Column::extract_data(const uint8* data) const throw () {
  // Used for temporary formatting of some data types
  // Should be grown as needed, starts with 30 bytes
  static unsigned int buffer_size = 30;
  static char* buffer = new char[buffer_size];

  const uint8* this_data = data + offset;

  switch(type){
    case COLUMN_UNKNOWN:
      return "unknown";

    // Booleans
    case COLUMN_BOOL:
      return (this_data[0] ? "True" : "False");

    // Integer types
    case COLUMN_INT8:
      return signed_to_string(get_int8(this_data), buffer, buffer_size);

    case COLUMN_UINT8:
      return unsigned_to_string(get_uint8(this_data), buffer, buffer_size);

    case COLUMN_INT16:
      return signed_to_string(get_int16(this_data), buffer, buffer_size);

    case COLUMN_UINT16:
      return unsigned_to_string(get_uint16(this_data), buffer, buffer_size);

    case COLUMN_INT32:
      return signed_to_string(get_int32(this_data), buffer, buffer_size);

    case COLUMN_UINT32:
      return unsigned_to_string(get_uint32(this_data), buffer, buffer_size);

    // String types
    case COLUMN_BLOB:
      // A blob requires 2 bytes per character, plus the NULL terminator
      if (buffer_size < (3* length) + 1){
        delete[] buffer;
        buffer = new char[(3 * length) + 1];
        buffer_size = (3 * length) + 1;
      }

      // For each byte in the blob, format it as a hex pair into the buffer
      for (unsigned int ii = 0; ii < length; ii++){
        sprintf(buffer + (ii * 3), "%02X ", this_data[ii]);
      }

      // NULL-terminate the string
      buffer[(3 * length) - 1] = 0;

      return buffer;

    case COLUMN_STRING:
      // If the string is NULL-terminated, it can be returned directly without
      // wasting any memory
      for (unsigned int ii = 0; ii < length; ii++){
        if (this_data[ii] == 0){
          return reinterpret_cast<const char*>(this_data);
        }
      }

      // Expand the buffer to string + 1 bytes
      if (buffer_size < length + 1){
        delete[] buffer;
        buffer = new char[length + 1];
        buffer_size = length + 1;
      }

      // Copy the string
      memcpy(buffer, this_data, length);

      // NULL-terminate the string
      buffer[length] = 0;

      return buffer;

    // FIXME: Dentrix-specific
    case COLUMN_VARSTRING: {
      // Length of the string
      // 2 == the offset from the start of the row to the length of the entire
      // row
      uint32 string_length = get_uint32(data + 2) - offset;

      // If the buffer is too small, resize it
      if (buffer_size < string_length + 1){
        delete[] buffer;
        buffer = new char[string_length + 1];
        buffer_size = string_length + 1;
      }

      // Copy data from the file into the buffer
      memcpy(buffer, this_data, string_length);

      // NULL-terminate the string
      buffer[string_length] = 0;

      return buffer;
    }

    // Special types

    // FIXME: specific to Dentrix?
    case COLUMN_PHONE:
      buffer[0] = this_data[0];
      buffer[1] = this_data[1];
      buffer[2] = this_data[2];
      buffer[3] = '-';
      buffer[4] = this_data[3];
      buffer[5] = this_data[4];
      buffer[6] = this_data[5];

      // If the phone number is 10 digits (123-456-7890)
      if (this_data[7]){
        buffer[7] = '-';
        buffer[8] = this_data[6];
        buffer[9] = this_data[7];
        buffer[10] = this_data[8];
        buffer[11] = this_data[9];
        buffer[12] = 0;
      }

      // Only 7 digits (123-4567)
      else {
        buffer[7] = this_data[6];
        buffer[8] = 0;
      }
      return buffer;

    // Output a date to YYYY-MM-DD format
    // FIXME: specific to Dentrix?
    case COLUMN_DATE: {
      // Number of days since 1700-02-28
      uint32 date_delta = get_uint32(this_data);

      // 1700-02-28, in Julian days
      uint32 julian_start_date = 2342031;

      uint32 julian_date = julian_start_date + date_delta;

      uint32 year;
      uint8 month, day;

      // Convert the julian date to YYYY-MM-DD format
      // This algorithm is from the glib library
      uint32 A, B, C, D, E, M;

      A = julian_date + 32045;
      B = (4 * (A + 36524)) / 146097 - 1;
      C = A - (146097 * B) / 4;
      D = (4 * (C + 365)) / 1461 - 1;
      E = C - ((1461*D) / 4);
      M = (5 * (E - 1) + 2)/153;

      month = static_cast<uint8>(M + 3 - (12*(M/10)));
      day = static_cast<uint8>(E - (153*M + 2)/5);
      year = 100 * B + D - 4800 + (M / 10);

      // Convert the values into a string
      sprintf(buffer, "%u-%u-%u", year, month, day);

      return buffer;
    }

    // A currency, with two decimal places
    // FIXME: specific to Dentrix?
    // FIXME: better implemented as a decimal type with settable accuracy?
    case COLUMN_CURRENCY: {
      float value = static_cast<float>(get_int32(this_data)) / 100.0f;

      snprintf(buffer, buffer_size-1, "%#.2f", value);

      return buffer;
    }

    // Enumeration type
    case COLUMN_ENUM: {
      return enumeration.get_value(get_uint8(this_data)).c_str();
    }

    // Bad types
    default:
      return "";
  }
}

} // namespace
