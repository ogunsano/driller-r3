/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * column_type_editor_creator.cpp
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

#include "column_type_editor_creator.h"

#include <QStringList>
#include "column_type_editor.h"

namespace Driller {

ColumnTypeEditorCreator::ColumnTypeEditorCreator(){}

QWidget* ColumnTypeEditorCreator::createWidget(QWidget* parent) const {
  ColumnTypeEditor* editor = new ColumnTypeEditor(parent);
  editor->setFrame(false);
  return editor;
}

QByteArray ColumnTypeEditorCreator::valuePropertyName() const {
  return "currentType";
}

} // namespace
