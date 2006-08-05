/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * column_type_editor.cpp
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

#include "column_type_editor.h"
#include <QStringList>

namespace Driller {

ColumnTypeEditor::ColumnTypeEditor(QWidget* parent):
  QComboBox(parent){

  for (unsigned int ii = 0; ii < COLUMN_NUM_TYPES; ii++){
    addItem(column_strings[ii].c_str(), ii);
  }
}

QtColumnType ColumnTypeEditor::getCurrentType() const {
  return static_cast<Driller::ColumnType>(currentIndex());
}

void ColumnTypeEditor::setCurrentType(const QtColumnType new_value){
  type = new_value.value;
  setCurrentIndex(type);
}

} // namespace
