/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * extracted_data_window.h
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

#ifndef DRILLER_QT_EXTRACTED_DATA_WINDOW_H
#define DRILLER_QT_EXTRACTED_DATA_WINDOW_H

#ifdef HAVE_CONFIG_H
  #include <config.h>
#endif

#include <QStringListModel>
#include <QList>
#include "ui_ExtractedDataWindow.h"
#include "../data_sink.h"

namespace Driller {

class ResultModel;

class ExtractedDataWindow : public QMainWindow, public DataSink {
  Q_OBJECT
public:
  /**
    Create a new window to hold extracted data

    @param parent The parent widget of this window
  */
  ExtractedDataWindow(QWidget* parent = NULL);

  /** Default destructor */
  ~ExtractedDataWindow();

  /**
    Extract the data from a table into this window, and then display it

    @param table The table to extract
  */
  void output_table(const Table& table, const unsigned int row_limit = 0)
    throw (Errors::FileReadError);

protected:
  Ui::ExtractedDataWindow ui;

  QList<ResultModel*> results;

  QStringListModel table_list;

protected slots:
  void on_closeButton_pressed();

  void set_visible_result(const QModelIndex& index);
};

} // namespace

#endif // DRILLER_QT_EXTRACTED_DATA_WINDOW_H
