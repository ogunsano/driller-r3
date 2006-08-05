/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * qt_column_type.h
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

#ifndef DRILLER_QT_COLUMN_TYPE_H
#define DRILLER_QT_COLUMN_TYPE_H

#include <QObject>
#include <QMetaType>
#include "../database/column.h"

/**
  A wrapper around Driller::ColumnType to be able to fit into a QVariant. 
  Putting this in a namespace seems to break Qt's QVariant code
*/
class QtColumnType : public QObject {
  Q_OBJECT

public:
  QtColumnType(const Driller::ColumnType& _value = Driller::COLUMN_UNKNOWN);

  QtColumnType(const QtColumnType& other);

  QtColumnType& operator=(const QtColumnType& other);

  Driller::ColumnType value;
};

Q_DECLARE_METATYPE(QtColumnType)

#endif // DRILLER_QT_COLUMN_TYPE_H
