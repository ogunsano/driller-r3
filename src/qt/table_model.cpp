/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * table_model.cpp
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

#include "table_model.h"
#include "main_window.h"
#include "column_type_editor.h"
#include "column_type_editor_creator.h"

namespace Driller {

static const int COLUMN_COUNT = 5;
static const char* headers[COLUMN_COUNT] = {"Name", "Type", "Offset", "Length", "Indexed"};

/**
  Types of QModelIndex
*/
typedef enum {
  INDEX_INVALID,
  INDEX_COLUMN,
  INDEX_ENUM_CASE,

  INDEX_COUNT
} IndexType;

/**
  This struct is designed to fit in a qint64, to serve as a more detailed
  QModelIndex
*/
struct IndexData {
  IndexData():
  type(INDEX_INVALID), parent_index(0){}

  IndexData(qint64 value){

    parent_index = value;
    type = static_cast<IndexType>(value);
    value >>= 16;

    type = static_cast<IndexType>(value);
  }

  qint64 to_qint64(){
    qint64 value = type;

    value <<= 16;
    value += parent_index;

    return value;
  }

  /** Whether this index is a column or enumeration case */
  IndexType type : 8;

  /**
    If this index is an enumeration type, this contains the index of the
    parent column
  */
  quint16 parent_index;
};

TableModel::TableModel(QObject* parent):
  QAbstractItemModel(parent), table(NULL){}

TableModel::~TableModel(){}

int TableModel::rowCount(const QModelIndex& idx) const {
  if (!table){
    return 0;
  }

  // Root index
  if (!idx.isValid()){
    return table->column_count();
  }

  IndexData data(idx.internalId());
  if (data.type == INDEX_COLUMN){
    const Column& col = table->column_at(idx.row());
    if (col.get_type() == COLUMN_ENUM){
      return col.enumeration.case_count();
    }
  }

  return 0;
}

int TableModel::columnCount(const QModelIndex& parent) const{
  // Root index
  if (!parent.isValid()){
    return COLUMN_COUNT;
  }

  IndexData parent_data(parent.internalId());

  if (parent_data.type == INDEX_COLUMN){
    return 2;
  }

  return COLUMN_COUNT;
}

QVariant TableModel::data(const QModelIndex& idx, int role) const {
  if (!table)
    return QVariant();

  if (!idx.isValid())
    return QVariant();

  IndexData data(idx.internalId());

  if (data.type == INDEX_COLUMN){
    if (!(
      role == Qt::DisplayRole ||
      role == Qt::EditRole ||
      (role == Qt::CheckStateRole && idx.column() == 4)))
      return QVariant();

    const Column& col = table->column_at(idx.row());

    switch (idx.column()){
      // Name
      case 0:
        return col.get_name().c_str();

      // Type
      case 1:
        if (role == Qt::DisplayRole){
          return column_strings[col.get_type()].c_str();
        }

        else {
          return QVariant::fromValue(QtColumnType(col.get_type()));
        }

      // Offset
      case 2:
        return col.get_offset();

      // Length
      case 3:
        if (!(col.get_type() == COLUMN_BLOB ||
          col.get_type() == COLUMN_STRING)){

          return QVariant();
        }
        return col.get_length();

      // Indexed
      case 4:
        if (role == Qt::CheckStateRole){
          return (col.get_indexed()? Qt::Checked : Qt::Unchecked);
        }

        else { // role == Qt::EditRole || role == Qt::DisplayRole
          return (col.get_indexed()? "True" : "False");
        }
    }
  }

  // Enumeration cases
  else {
    if (!(
      role == Qt::DisplayRole ||
      role == Qt::EditRole))
      return QVariant();

    const Column& col = table->column_at(data.parent_index);

    std::list<EnumCase> case_list = col.enumeration.get_case_list();
    std::list<EnumCase>::iterator iter = case_list.begin();

    for (int ii = 0; ii < idx.row(); ii++) iter++;

    switch (idx.column()){
      // Enumeration ID
      case 0:
        return (*iter).id;

      // Enumeration value
      case 1:
        return (*iter).value.c_str();

      default:
        return QVariant();
    }
  }

  return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation, int role) const {
  if (role != Qt::DisplayRole) return QVariant();

  if (section >= 0 && section < COLUMN_COUNT)
    return headers[section];
  else
    return QVariant();
}

bool TableModel::setData(const QModelIndex& idx, const QVariant& value, int){
  if (!table)
    return false;

  if (!idx.isValid())
    return false;

  IndexData data(idx.internalId());
  if (data.type == INDEX_COLUMN){
    Column& col = table->column_at(idx.row());

    switch (idx.column()){
      // Name
      case 0:
        col.set_name(value.toString().toUtf8().constData());
        break;

      // Type
      case 1: {
        QtColumnType type = qvariant_cast<QtColumnType>(value);
        col.set_type(type.value);
        break;
      }

      // Offset
      case 2:
        col.set_offset(value.toUInt());
        break;

      // Length
      case 3:
        if (col.get_type() == COLUMN_BLOB || col.get_type() == COLUMN_STRING)
          col.set_length(value.toUInt());
        else
          return false;
        break;

      // Indexed
      case 4:
        col.set_indexed(value.toBool());
        break;
    }
    return true;
  }

  // INDEX_ENUM_CASE
  else {
    Enumeration& enumeration = table->column_at(data.parent_index).enumeration;

    std::list<EnumCase> case_list = enumeration.get_case_list();
    std::list<EnumCase>::iterator iter = case_list.begin();

    for (int ii = 0; ii < idx.row(); ii++) iter++;

    switch (idx.column()){
      // ID
      case 0:
        try {
          enumeration.change_id((*iter).id, value.toUInt());
        }

        /*
          The reason this isn't being caught properly in MainWindow is because
          this function is actually called from the QApplication event loop.
          If you can find some way to re-start the event loop, put this catch
          code in QtGUI
        */
        catch (const Errors::BaseError& e){
          MainWindow* active_window = qobject_cast<MainWindow*>(
            QApplication::activeWindow());

          if (active_window){
            active_window->show_error(e);
          }
        }
        break;

      // Value
      case 1:
        enumeration.change_value((*iter).id, value.toString().toUtf8().
          constData());
        break;
    }
  }

  return false;
}

QModelIndex TableModel::index (int row, int column,
  const QModelIndex& parent) const {

  // Getting the index of a column
  if (!parent.isValid()){
    IndexData data;
    data.type = INDEX_COLUMN;
    data.parent_index = row;
    return createIndex(row, column, (void*)data.to_qint64());
  }

  // Possibly getting the index of an enumeration
  else {
    IndexData parent_data(parent.internalId());

    if (parent_data.type == INDEX_COLUMN){
      IndexData data;
      data.type = INDEX_ENUM_CASE;
      data.parent_index = parent.row();
      return createIndex(row, column, (void*)data.to_qint64());
    }
  }

  return QModelIndex();
}

QModelIndex TableModel::parent(const QModelIndex& child) const {
  if (!child.isValid()){
    return QModelIndex();
  }

  IndexData data(child.internalId());

  if (data.type == INDEX_ENUM_CASE){
    IndexData parent_data;
    parent_data.type = INDEX_COLUMN;

    return createIndex(data.parent_index, 0, (void*)parent_data.to_qint64());
  }

  return QModelIndex();
}

Qt::ItemFlags TableModel::flags(const QModelIndex& idx) const {
  if (!table)
    return 0;

  if (!idx.isValid())
    return 0;

  IndexData data(idx.internalId());

  if (data.type == INDEX_COLUMN){
    const Column& col = table->column_at(idx.row());

    // Special case the length column, since not all column types may have
    // a custom length
    if (idx.column() == 3){
      if (!(col.get_type() == COLUMN_BLOB ||
        col.get_type() == COLUMN_STRING)){

          return Qt::ItemIsSelectable;
      }
    }
  }

  Qt::ItemFlags column_flags = Qt::ItemIsSelectable | Qt::ItemIsEditable |
        Qt::ItemIsEnabled;

  // Allow checking of the index column
  if (idx.column() == 4)
    column_flags |= Qt::ItemIsUserCheckable;

  return column_flags;
}

void TableModel::set_table(Table* _table){
  table = _table;

  reset();
}

bool TableModel::insertRow(int row, const QModelIndex& parent){
  beginInsertRows(parent, row, row);

  if (parent.isValid()){    
    IndexData parent_data(parent.internalId());

    if (parent_data.type == INDEX_COLUMN){
      Enumeration& enumeration = table->column_at(parent.row()).enumeration;
      enumeration.add_case("");
    }
  }

  else {
    table->add_column(Column());
  }

  endInsertRows();
  return true;
}

bool TableModel::removeRow(int row, const QModelIndex& parent){
  beginRemoveRows(QModelIndex(), row, row);

  if (parent.isValid()){
    IndexData parent_data(parent.internalId());
    
    if (parent_data.type == INDEX_COLUMN){
      Enumeration& enumeration = table->column_at(parent.row()).enumeration;

      std::list<EnumCase> case_list = enumeration.get_case_list();
      std::list<EnumCase>::iterator iter = case_list.begin();

      for (int ii = 0; ii < row; ii++) iter++;

      enumeration.remove_id((*iter).id);
    }
  }

  else {
    if (row < 0 || row > static_cast<int>(table->column_count()))
      return false;

    table->remove_column(row);
    endRemoveRows();
  }

  return true;
}

bool TableModel::is_enum_case(const QModelIndex& index){
  IndexData data(index.internalId());

  return (data.type == INDEX_ENUM_CASE);
}

} // namespace
