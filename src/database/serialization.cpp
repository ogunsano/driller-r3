#include "database.h"
#include <libxml/xmlwriter.h>
#include <sstream>

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

std::ostream& operator<<(std::ostream& out, const Database& db)
  throw (Errors::FileParseError) {

  init_libxml();

  xmlBuffer* buf = xmlBufferCreate();

  if (!buf){
    throw Errors::FileParseError("");
  }

  xmlTextWriter* writer = xmlNewTextWriterMemory(buf, 0);

  if (!writer){
    throw Errors::FileParseError("");
  }

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

  xmlFreeTextWriter(writer);
  out << buf->content;
  xmlBufferFree(buf);

  return out;
}

} // namespace
