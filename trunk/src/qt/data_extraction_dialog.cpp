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

#include <QCheckBox>
#include <QSpinBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QProgressDialog>

#include "data_extraction_dialog.h"
#include "extracted_data_window.h"
#include "../file_sink.h"

#if ENABLE_MYSQL
#include "../mysql_sink.h"
#endif

namespace Driller {

/** Available output types */
enum OutputType {
  /** Output to a window */
  OUTPUT_WINDOW,

  /** Output to several text files */
  OUTPUT_TEXT,

#if ENABLE_MYSQL
  /** Output to a MySQL database */
  OUTPUT_MYSQL,
#endif

  /** How many output types are available */
  OUTPUT_COUNT
};

/** String for the output types */
static const QString output_strings[OUTPUT_COUNT] = {
  "Temporary window", "Text files",
#if ENABLE_MYSQL
  "MySQL database"
#endif
};

DataExtractionDialog::DataExtractionDialog(QWidget* parent):
  QDialog(parent){

  // Create the user interface

  // Set the layout
  QVBoxLayout* vbox = new QVBoxLayout(this);

  // Options for all output types
  {
    QHBoxLayout* hbox = new QHBoxLayout;
    vbox->addLayout(hbox);

    QLabel* label = new QLabel("How many rows?", this);
    row_count = new QSpinBox(this);
    row_count->setRange(1, 100000000); // 100 million ought to be enough

    all_rows = new QCheckBox("Extract all rows", this);
    hbox->addWidget(label);
    hbox->addWidget(row_count);
    hbox->addWidget(all_rows);

    // Connect signals
    QObject::connect(all_rows, SIGNAL(stateChanged(int)),
      this, SLOT(on_all_rows_stateChanged(int)));
  }

  // Output selection
  {
    QHBoxLayout* hbox = new QHBoxLayout;
    vbox->addLayout(hbox);

    QLabel* label = new QLabel("Output type:", this);
    hbox->addWidget(label);

    output_selection = new QComboBox();

    // Add output types
    for (int ii = 0; ii < OUTPUT_COUNT; ii++) {
      output_selection->addItem(output_strings[ii], ii);
    }
    hbox->addWidget(output_selection);

    // Connect signals
    QObject::connect(output_selection, SIGNAL(currentIndexChanged(int)),
      this, SLOT(on_output_selection_currentIndexChanged(int)));
  }

  // Options for text output
  {
    text_options = new QGroupBox(this);
    text_options->setTitle("Text output options");
    text_options->hide();
    vbox->addWidget(text_options);

    QHBoxLayout* hbox = new QHBoxLayout(text_options);
    text_options->setLayout(hbox);

    QLabel* label = new QLabel("Output path:", text_options);
    text_output_path = new QLineEdit(text_options);

    text_output_path_browse = new QPushButton("Browse", text_options);

    hbox->addWidget(label);
    hbox->addWidget(text_output_path);
    hbox->addWidget(text_output_path_browse);

    // Connect signals
    QObject::connect(text_output_path_browse, SIGNAL(clicked(bool)),
      this, SLOT(find_text_output_path()));
  }

  // Options for MySQL output
  {
    mysql_options = new QGroupBox(this);
    mysql_options->hide();
    vbox->addWidget(mysql_options);

#if ENABLE_MYSQL
    mysql_options->setTitle("MySQL options");

    QGridLayout* grid = new QGridLayout(mysql_options);
    mysql_options->setLayout(grid);

    mysql_host_name = new QLineEdit(mysql_options);
    mysql_port = new QSpinBox(mysql_options);
    mysql_user_name = new QLineEdit(mysql_options);

    mysql_password = new QLineEdit(mysql_options);
    mysql_password->setEchoMode(QLineEdit::Password);

    mysql_database_name = new QLineEdit(mysql_options);

    // Sensible defaults for some of the options
    mysql_host_name->setText("localhost");
    mysql_port->setMinimum(1);
    mysql_port->setMaximum(65535);
    mysql_port->setValue(3306);
    // Try to get the user name from the OS
    const char* env_USER = getenv("USER");
    if (env_USER) {
      mysql_user_name->setText(env_USER);
    }

    // Labels used for the options
    QLabel
      *host_name_label = new QLabel("Host:", mysql_options),
      *port_label = new QLabel("Port:", mysql_options),
      *user_name_label = new QLabel("User name:", mysql_options),
      *password_label = new QLabel("Password:", mysql_options),
      *database_name_label = new QLabel("Database:", mysql_options);

    // Add widgets to the layout
    grid->addWidget(mysql_host_name, 0, 1);
    grid->addWidget(mysql_port, 1, 1);
    grid->addWidget(mysql_user_name, 2, 1);
    grid->addWidget(mysql_password, 3, 1);
    grid->addWidget(mysql_database_name, 4, 1);

    grid->addWidget(host_name_label, 0, 0);
    grid->addWidget(port_label, 1, 0);
    grid->addWidget(user_name_label, 2, 0);
    grid->addWidget(password_label, 3, 0);
    grid->addWidget(database_name_label, 4, 0);

#endif
  }

  // Buttons to accept or cancel the dialog
  {
    QHBoxLayout* hbox = new QHBoxLayout;
    vbox->addLayout(hbox);

    QPushButton* extract, *cancel;

    extract = new QPushButton("Extract");
    cancel = new QPushButton("Cancel");

    hbox->addWidget(extract);
    hbox->addWidget(cancel);

    // Connect signals
    QObject::connect(extract, SIGNAL(clicked(bool)),
      this, SLOT(accept()));

    // Connect signals
    QObject::connect(cancel, SIGNAL(clicked(bool)),
      this, SLOT(reject()));
  }
}

DataExtractionDialog::~DataExtractionDialog(){
}

void DataExtractionDialog::exec(const Database& db){
  // Return early if the user cancels the dialog
  if (QDialog::exec() == QDialog::Rejected) return;

  const int output_type = output_selection->itemData(
    output_selection->currentIndex()).toInt();

  unsigned int row_limit = (all_rows->checkState()? 0 : row_count->value());

  DataSink* sink = 0;

  switch (output_type) {
    case OUTPUT_TEXT:
      sink = new FileSink(text_output_path->text().toUtf8().constData());
      break;

#if ENABLE_MYSQL
    case OUTPUT_MYSQL:
      sink = new MySQLSink(
        mysql_host_name->text().toUtf8().constData(),
        mysql_user_name->text().toUtf8().constData(),
        mysql_password->text().toUtf8().constData(),
        mysql_database_name->text().toUtf8().constData(),
        mysql_port->value()
      );
      break;
#endif

    default:
      sink = new ExtractedDataWindow(parentWidget());
  }

  unsigned int ii = 0;
  std::vector<Table> tables = db.get_tables();
  std::vector<Table>::const_iterator table;

  QProgressDialog progress("Extracting data", "Cancel", 0, tables.size(), this);

  try {
    for (table = tables.begin(); table != tables.end(); table++){
      QCoreApplication::processEvents();
      if (progress.wasCanceled()) break;
      sink->output_table(*table, row_limit);
      progress.setValue(++ii);
    }
  }

  catch (...) {
    delete sink;
    throw;
  }
}

void DataExtractionDialog::on_all_rows_stateChanged(int state){
  if (state == Qt::Checked){
    // Disable the row count spinbox
    row_count->setEnabled(false);
  }

  else {
    row_count->setEnabled(true);
  }
}

void DataExtractionDialog::find_text_output_path() {
  QString dir = QFileDialog::getExistingDirectory(
    this, "Find output path", text_output_path->text(),
    QFileDialog::ShowDirsOnly
  );

  if (dir.length() > 0) {
    text_output_path->setText(dir);
  }
}

void DataExtractionDialog::on_output_selection_currentIndexChanged(
  const int index) {

  const int output_type = output_selection->itemData(index).toInt();
  if (output_type < 0 || output_type >= OUTPUT_COUNT) {
    return;
  }

  switch (output_type) {
    case OUTPUT_TEXT:
      text_options->show();
      mysql_options->hide();
      break;

#if ENABLE_MYSQL
    case OUTPUT_MYSQL:
      text_options->hide();
      mysql_options->show();
      break;
#endif

    default:
      text_options->hide();
      mysql_options->hide();
  }
}

} // namespace
