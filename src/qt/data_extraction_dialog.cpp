/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * data_extraction_dialog.cpp
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

#include "data_extraction_dialog.h"
#include "extracted_data_window.h"

namespace Driller {

DataExtractionDialog::DataExtractionDialog(QWidget* parent):
  QDialog(parent){

  ui.setupUi(this);
}

DataExtractionDialog::~DataExtractionDialog(){
}

void DataExtractionDialog::set_database(const Database& _db){
  db = &_db;
}

void DataExtractionDialog::exec(){
  // Return early if the user cancels the dialog
  if (QDialog::exec() == QDialog::Rejected) return;

  unsigned int row_limit = (ui.allRows->checkState()? 0 : ui.rowCount->value());
  ExtractedDataWindow* extracted_data = new ExtractedDataWindow(parentWidget(),
    row_limit);
  (*extracted_data) << (*db);
  extracted_data->show();
}

void DataExtractionDialog::on_allRows_stateChanged(int state){
  if (state == Qt::Checked){
    // Disable the row count spinbox
    ui.rowCount->setEnabled(false);
  }

  else {
    ui.rowCount->setEnabled(true);
  }
}

} // namespace
