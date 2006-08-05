/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * table_model.h
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

#ifndef DRILLER_QT_TABLE_MODEL_H
#define DRILLER_QT_TABLE_MODEL_H

#include <QAbstractItemModel>
#include "../database.h"

namespace Driller {

class TableModel : public QAbstractItemModel {
  Q_OBJECT

public:
  TableModel(QObject* parent = NULL);
  ~TableModel();

  int rowCount(const QModelIndex& parent = QModelIndex()) const;
  int columnCount(const QModelIndex& parent = QModelIndex()) const;

  QVariant data(const QModelIndex& index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation,
    int role = Qt::DisplayRole) const;

  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

  QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
  QModelIndex parent(const QModelIndex& index) const;

  Qt::ItemFlags flags(const QModelIndex& index) const;

  void set_table(Table* table);

  bool insertRow(int row, const QModelIndex& parent = QModelIndex());
  bool removeRow(int row, const QModelIndex& parent = QModelIndex());

  bool is_enum_case(const QModelIndex& index);

protected:
  Table* table;
};

} // namespace

#endif // DRILLER_QT_TABLE_MODEL_H
