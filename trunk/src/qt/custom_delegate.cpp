/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * custom_delegate.cpp
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

#include "custom_delegate.h"
#include <QCheckBox>
#include <QAbstractItemModel>
#include <QPainter>

namespace Driller {

CustomDelegate::CustomDelegate(QObject* parent):
  QItemDelegate(parent){}

void CustomDelegate::paint(QPainter* painter,
  const QStyleOptionViewItem& option,
  const QModelIndex& index) const {

  if (!index.isValid())
    return;
  QVariant value = index.data(Qt::EditRole);

  // Copied from the Qt source code
  // qitemdelegate.cpp
  // QItemDelegate::paint
  if (value.type() == QVariant::Bool){

    Qt::CheckState checkState;
    if (value.toBool())
      checkState = Qt::Checked;
    else
      checkState = Qt::Unchecked;

    QRect checkRect = check(option, option.rect, checkState);

    // draw the background color
    if (option.showDecorationSelected && (option.state & QStyle::State_Selected)) {
        QPalette::ColorGroup cg = option.state & QStyle::State_Enabled
                                  ? QPalette::Normal : QPalette::Disabled;
        if (cg == QPalette::Normal && !(option.state & QStyle::State_Active))
            cg = QPalette::Inactive;
        painter->fillRect(option.rect, option.palette.brush(cg, QPalette::Highlight));
    } else {
        value = index.data(Qt::BackgroundColorRole);
        if (value.isValid() && qvariant_cast<QColor>(value).isValid())
            painter->fillRect(option.rect, qvariant_cast<QColor>(value));
    }

    // draw the item
    drawCheck(painter, option, checkRect, checkState);
    
  }

  else {
    QItemDelegate::paint(painter, option, index);
  }
}

QWidget* CustomDelegate::createEditor(QWidget* parent,
  const QStyleOptionViewItem& view_item,
  const QModelIndex& index) const {

  if (!index.isValid())
    return NULL;
  QVariant::Type t = index.data(Qt::EditRole).type();

  // If boolean, return a checkbox
  if (t == QVariant::Bool){
    QCheckBox* box = new QCheckBox(parent);
    box->installEventFilter(const_cast<CustomDelegate*>(this));

    return box;
  }

  // If not, use whatever QItemDelegate thinks is best
  else {
    return QItemDelegate::createEditor(parent, view_item, index);
  }
}

void CustomDelegate::setEditorData(QWidget* editor,
  const QModelIndex& index) const {

  if (!index.isValid())
    return;

  QVariant value = index.data(Qt::EditRole);

  if (value.type() == QVariant::Bool){
    bool checked = value.toBool();

    QCheckBox* box = static_cast<QCheckBox*>(editor);
    box->setCheckState(checked? Qt::Checked : Qt::Unchecked);
  }

  else {
    QItemDelegate::setEditorData(editor, index);
  }
}

void CustomDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
  const QModelIndex& index) const {

  if (!index.isValid())
    return;
  QVariant::Type t = index.data(Qt::EditRole).type();

  if (t == QVariant::Bool){
    QCheckBox* box = static_cast<QCheckBox*>(editor);
    model->setData(index, (box->checkState() == Qt::Checked));
  }

  else {
    QItemDelegate::setModelData(editor, model, index);
  }
}

void CustomDelegate::updateEditorGeometry(QWidget* editor,
  const QStyleOptionViewItem& option, const QModelIndex& index) const {

  if (!index.isValid())
    return;
  QVariant::Type t = index.data(Qt::EditRole).type();

  if (t == QVariant::Bool){
    editor->setGeometry(option.rect);
  }

  else {
    QItemDelegate::updateEditorGeometry(editor, option, index);
  }
}

}
