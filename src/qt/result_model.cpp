/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * result_model.cpp
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

#include "result_model.h"

namespace Driller {

ResultModel::ResultModel(const ResultSet* _result, const Table* table,
  QObject* parent):
  QAbstractListModel(parent), result(_result){

  // Get the header strings from the table
  std::vector<Column> columns = table->get_columns();
  std::vector<Column>::iterator iter;
  for (iter = columns.begin(); iter != columns.end(); iter++){
    header_list << iter->get_name().c_str();
  }
}

ResultModel::~ResultModel(){
  delete result;
}

int ResultModel::rowCount(const QModelIndex&) const {
  return result->row_count();
}

int ResultModel::columnCount(const QModelIndex&) const{
  return result->column_count();
}

QVariant ResultModel::data(const QModelIndex& index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole)
    return QVariant();

  return (*result)[index.row()][index.column()];
}

QVariant ResultModel::headerData(int section, Qt::Orientation, int role) const {
  if (role != Qt::DisplayRole)
    return QVariant();

  return header_list.at(section);
}

QModelIndex ResultModel::index (int row, int column, const QModelIndex&) const {
//  return createIndex(row, column, &(table->columns.at(row)));
  return createIndex(row, column, 0);
}

QModelIndex ResultModel::parent (const QModelIndex&) const {
  return QModelIndex();
}

Qt::ItemFlags ResultModel::flags(const QModelIndex& index) const {
  if (!index.isValid())
    return 0;

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

} // namespace
