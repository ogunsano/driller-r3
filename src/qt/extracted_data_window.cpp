/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * extracted_data_window.cpp
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

#include "extracted_data_window.h"
#include <QTreeView>
#include "result_model.h"

namespace Driller {

ExtractedDataWindow::ExtractedDataWindow(QWidget* parent):
  QMainWindow(parent) {

  ui.setupUi(this);

  table_list.insertColumns(0, 1);
  ui.tableView->setModel(&table_list);

  connect(
    ui.tableView->selectionModel(),
    SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
    this, SLOT(set_visible_result(const QModelIndex&))
  );
}

ExtractedDataWindow::~ExtractedDataWindow(){
  QList<ResultModel*>::iterator iter;
  for (iter = results.begin(); iter != results.end(); iter++){
    delete (*iter);
  }
}

void ExtractedDataWindow::output_table(const Table& table,
  const unsigned int row_limit) throw (Errors::FileReadError) {

  // Extract the data
  const ResultSet* result = table.extract_data(row_limit);

  // Display the model
  ResultModel* model = new ResultModel(result, &table);
  results.append(model);

  table_list.insertRows(table_list.rowCount(), 1);
  table_list.setData(table_list.index(table_list.rowCount()-1, 0),
    table.get_name().c_str());

  // If first table to be extracted to this window, select it
  if (results.size() == 1) {
    ui.tableView->selectionModel()->select(table_list.index(0,0),
      QItemSelectionModel::Rows | QItemSelectionModel::SelectCurrent);
    ui.resultView->setModel(results.at(0));
  }

  // Show the window
  show();
}

void ExtractedDataWindow::on_closeButton_pressed(){
  delete this;
}

void ExtractedDataWindow::set_visible_result(const QModelIndex& index){
  if (index.isValid()){
    if (index.row() < results.size()){
      ui.resultView->setModel(results.at(index.row()));
    }
  }
}

} // namespace
