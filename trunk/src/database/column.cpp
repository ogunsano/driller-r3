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
#include "../file_errors.h"
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

/** Holds information on extracting data from a column */
class ColumnExtractionInfo {
public:
  ColumnExtractionInfo(const uint8* _data,
                       const unsigned int _length,
                       const unsigned int _offset,
                       const Enumeration& _enumeration,
                       char*& _buffer,
                       unsigned int& _buffer_size):

                       data(_data),
                       length(_length),
                       offset(_offset),
                       enumeration(_enumeration),
                       buffer(_buffer),
                       buffer_size(_buffer_size) {}

  const uint8* data;
  const unsigned int length;
  const unsigned int offset;
  const Enumeration& enumeration;
  char*& buffer;
  unsigned int& buffer_size;
};

/**
  Typedef for a data extraction function

  @param extract_struct Holds information on extracting data from a column

  @param data The data to extract
*/
typedef const char*(*ColumnExtractionFunc)(const ColumnExtractionInfo&);

/* Extraction functions. Each of these handles one data type */

/* COLUMN_UNKNOWN */
const char* extract_unknown(const ColumnExtractionInfo& info) {
  return "unknown";
}

/* COLUMN_BOOL */
const char* extract_bool(const ColumnExtractionInfo& info) {
  return info.data[0] ? "True" : "False";
}

/* COLUMN_INT8 */
const char* extract_int8(const ColumnExtractionInfo& info) {
  return signed_to_string(Column::get_int8(info.data), info.buffer, info.buffer_size);
}

/* COLUMN_UINT8 */
const char* extract_uint8(const ColumnExtractionInfo& info) {
  return unsigned_to_string(Column::get_uint8(info.data), info.buffer, info.buffer_size);
}

/* COLUMN_INT16 */
const char* extract_int16(const ColumnExtractionInfo& info) {
  return signed_to_string(Column::get_int16(info.data), info.buffer, info.buffer_size);
}

/* COLUMN_UINT16 */
const char* extract_uint16(const ColumnExtractionInfo& info) {
  return unsigned_to_string(Column::get_uint16(info.data), info.buffer, info.buffer_size);
}

/* COLUMN_INT32 */
const char* extract_int32(const ColumnExtractionInfo& info) {
  return signed_to_string(Column::get_int32(info.data), info.buffer, info.buffer_size);
}

/* COLUMN_UINT32 */
const char* extract_uint32(const ColumnExtractionInfo& info) {
  return unsigned_to_string(Column::get_uint32(info.data), info.buffer, info.buffer_size);
}

/* COLUMN_BLOB */
const char* extract_blob(const ColumnExtractionInfo& info) {
  // A blob requires 2 bytes per character, plus the NULL terminator
  if (info.buffer_size < (3* info.length) + 1){
    delete[] info.buffer;
    info.buffer = new char[(3 * info.length) + 1];
    info.buffer_size = (3 * info.length) + 1;
  }

  // For each byte in the blob, format it as a hex pair into the info.buffer
  for (unsigned int ii = 0; ii < info.length; ii++){
    sprintf(info.buffer + (ii * 3), "%02X ", info.data[ii]);
  }

  // NULL-terminate the string
  info.buffer[(3 * info.length) - 1] = 0;

  return info.buffer;
}

/* COLUMN_STRING */
const char* extract_string(const ColumnExtractionInfo& info) {
  // If the string is NULL-terminated, it can be returned directly without
  // wasting any memory
  for (unsigned int ii = 0; ii < info.length; ii++){
    if (info.data[ii] == 0){
      return reinterpret_cast<const char*>(info.data);
    }
  }

  // Expand the info.buffer to string + 1 bytes
  if (info.buffer_size < info.length + 1){
    delete[] info.buffer;
    info.buffer = new char[info.length + 1];
    info.buffer_size = info.length + 1;
  }

  // Copy the string
  memcpy(info.buffer, info.data, info.length);

  // NULL-terminate the string
  info.buffer[info.length] = 0;

  return info.buffer;
}

/* COLUMN_VARSTRING */
const char* extract_varstring(const ColumnExtractionInfo& info) {
  // Length of the string
  // 2 == the offset from the start of the row to the info.length of the entire
  // row
  uint32 string_length = Column::get_uint32(info.data + 2 - info.offset) - info.offset;

  // If the info.buffer is too small, resize it
  if (info.buffer_size < string_length + 1){
    char* new_buffer = new(std::nothrow) char[string_length + 1];

    // If this varstring is corrupt, it might try to allocate too much memory
    if (new_buffer) {
      delete[] info.buffer;
      info.buffer_size = string_length + 1;
      info.buffer = new_buffer;
    }

    else {
      return "Corrupt varstring";
    }
  }

  // Copy info.data from the file into the info.buffer
  memcpy(info.buffer, info.data, string_length);

  // NULL-terminate the string
  info.buffer[string_length] = 0;

  return info.buffer;
}

/* COLUMN_PHONE */
const char* extract_phone(const ColumnExtractionInfo& info) {
  info.buffer[0] = info.data[0];
  info.buffer[1] = info.data[1];
  info.buffer[2] = info.data[2];
  info.buffer[3] = '-';
  info.buffer[4] = info.data[3];
  info.buffer[5] = info.data[4];
  info.buffer[6] = info.data[5];

  // If the phone number is 10 digits (123-456-7890)
  if (info.data[7]){
    info.buffer[7] = '-';
    info.buffer[8] = info.data[6];
    info.buffer[9] = info.data[7];
    info.buffer[10] = info.data[8];
    info.buffer[11] = info.data[9];
    info.buffer[12] = 0;
  }

  // Only 7 digits (123-4567)
  else {
    info.buffer[7] = info.data[6];
    info.buffer[8] = 0;
  }
  return info.buffer;
}

/* COLUMN_DATE */
const char* extract_date(const ColumnExtractionInfo& info) {
  // Number of days since 1700-02-28
  uint32 date_delta = Column::get_uint32(info.data);

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
  sprintf(info.buffer, "%u-%u-%u", year, month, day);

  return info.buffer;
}

/* COLUMN_CURRENCY */
const char* extract_currency(const ColumnExtractionInfo& info) {
  float value = static_cast<float>(Column::get_int32(info.data)) / 100.0f;
  snprintf(info.buffer, info.buffer_size-1, "%#.2f", value);
  return info.buffer;
}

/* COLUMN_ENUM */
const char* extract_enum(const ColumnExtractionInfo& info) {
  const std::string& retval = info.enumeration.get_value(
    Column::get_uint8(info.data));
  const unsigned int retval_size = static_cast<unsigned int>(
    retval.size());

  // Expand the info.buffer to string + 1 bytes
  if (info.buffer_size < retval_size + 1){
    delete[] info.buffer;
    info.buffer = new char[retval_size + 1];
    info.buffer_size = retval_size + 1;
  }

  // Copy the string
  memcpy(info.buffer, retval.c_str(), retval_size);

  // NULL-terminate the string
  info.buffer[retval_size] = 0;

  return info.buffer;
}

/* Mapping from types to extraction functions */
const ColumnExtractionFunc extraction_function_map[COLUMN_NUM_TYPES] = {
  extract_unknown,
  extract_bool,
  extract_int8,
  extract_uint8,
  extract_int16,
  extract_uint16,
  extract_int32,
  extract_uint32,
  extract_blob,
  extract_string,
  extract_varstring,
  extract_phone,
  extract_date,
  extract_currency,
  extract_enum
};

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

  // Type of the column
  std::string raw_type = reinterpret_cast<const char*>(node->name);
  for (int ii = 0; ii < COLUMN_NUM_TYPES; ii++){
    if (raw_type == short_column_strings[ii]){
      type = static_cast<ColumnType>(ii);
    }
  }

  // Name of the column
  name = get_xml_node_attribute(node, "name");

  // Data offset
  std::string raw_offset = get_xml_node_attribute(node, "offset");

  // Indexed
  xmlChar* raw_indexed = xmlGetProp(node, BAD_CAST("indexed"));
  if (raw_indexed) {
    xmlFree(raw_indexed);
    indexed = (get_xml_node_attribute(node, "indexed") == "true");
  }

  else {
    indexed = false;
  }

  std::stringstream strstream;

  strstream << raw_offset;
  strstream >> offset;

  if (strstream.fail()) {
    throw Errors::InvalidAttributeError("", "offset", raw_offset);
  }

  if (needs_length()){
    std::string raw_length = get_xml_node_attribute(node, "length");

    strstream.str("");
    strstream.clear();
    strstream << raw_length;
    strstream >> length;

    if (strstream.fail() || length < 1) {
      throw Errors::InvalidAttributeError("", "length", raw_length);
    }
  }

  // If the column is an enumeration, parse all enumeration values
  if (type == COLUMN_ENUM){
    xmlNode* case_node = node->children;
    int id; // not uint8 because the >> operator thinks uint8 is a char
    for (; case_node; case_node = case_node->next){
      if (case_node->type == XML_ELEMENT_NODE){

        // Read the ID
        std::string raw_id = get_xml_node_attribute(case_node, "id");

        strstream.str("");
        strstream.clear();
        strstream << raw_id;
        strstream >> id;

        if (strstream.fail() || id < 0) {
          throw Errors::InvalidAttributeError("", "id", raw_id);
        }

        enumeration.add_case(
          static_cast<Driller::uint8>(id),
          get_xml_node_attribute(case_node, "value"));
      }
    }
  }
}

void Column::set_type(const ColumnType _type) throw(){
  if (_type >= COLUMN_NUM_TYPES) {
    type = COLUMN_UNKNOWN;
  }
  else {
    type = _type;
  }
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

const char* Column::extract_data(const uint8* data,
                                 char*& buffer,
                                 unsigned int& buffer_size) const throw () {

  if (type == COLUMN_NUM_TYPES) {
    return "Invalid column type";
  }

  else {
    ColumnExtractionInfo info(
      data + offset,
      length,
      offset,
      enumeration,
      buffer,
      buffer_size
    );

    return extraction_function_map[type](info);
  }
}

} // namespace
