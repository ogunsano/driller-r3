/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * custom_delegate.h
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

#ifndef DRILLER_QT_CUSTOM_DELEGATE_H
#define DRILLER_QT_CUSTOM_DELEGATE_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <QItemDelegate>

namespace Driller {

class CustomDelegate : public QItemDelegate {
  Q_OBJECT

public:
  CustomDelegate(QObject* parent = NULL);

  void paint(QPainter* painter,
    const QStyleOptionViewItem& option,
    const QModelIndex& index) const;
  QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
    const QModelIndex& index) const;

  void setEditorData(QWidget* editor, const QModelIndex& index) const;
  void setModelData(QWidget* editor, QAbstractItemModel* model,
    const QModelIndex& index) const;

  void updateEditorGeometry(QWidget* editor,
    const QStyleOptionViewItem& option, const QModelIndex& index) const;

};

} // namespace

#endif // DRILLER_QT_CHECK_BOX_DELEGATE_H
