/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * column_type_editor.h
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

#ifndef DRILLER_QT_COLUMN_TYPE_EDITOR_H
#define DRILLER_QT_COLUMN_TYPE_EDITOR_H

#include <QComboBox>
#include "qt_column_type.h"

namespace Driller {

class ColumnTypeEditor : public QComboBox {
  Q_OBJECT

public:

  Q_PROPERTY(
    QtColumnType currentType
    READ getCurrentType
    WRITE setCurrentType
  )

  ColumnTypeEditor(QWidget* parent = 0);

  QtColumnType getCurrentType() const;
  void setCurrentType(const QtColumnType value);

protected:
  ColumnType type;
};

} // namespace

#endif // DRILLER_QT_QSTRINGLIST_EDITOR_H
