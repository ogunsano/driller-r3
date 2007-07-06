/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * data_sink.cpp
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

#include "data_sink.h"

namespace Driller {

DataSink::DataSink(){}

DataSink::~DataSink(){}

void DataSink::output_database(const Database& db) {
  std::vector<Table> tables = db.get_tables();
  std::vector<Table>::const_iterator table;
  for (table = tables.begin(); table != tables.end(); table++){
    output_table(*table);
  }
}

} // namespace
