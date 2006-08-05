/* Driller, a data extraction program
 * Copyright (C) 2005 John Millikin

 * gtk-gui.cpp

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <iostream>
#include "gtk-gui.h"
#include "driller.h"
#include "binreloc.h"

namespace Driller {

GtkGUI::GtkGUI(App& _app, Database& _db)
: BaseGUI(_app, _db)
{
  init();
}

GtkGUI::~GtkGUI(){
//  gtk_widget_destroy(GTK_WIDGET(main_window));
}

void GtkGUI::append_entry(Entry& entry){
    GtkListStore* list_store = GTK_LIST_STORE(gtk_tree_view_get_model(entry_view));
    GtkTreeIter tree_iter;

    gtk_list_store_append (list_store, &tree_iter);
    gtk_list_store_set (list_store, &tree_iter,
      0, entry.name.c_str(),
      1, entry_type_strings[entry.type].c_str(),
      2, entry.offset,
      3, entry.length,
      4, entry.indexed,
      -1
    );
}

void GtkGUI::set_filename(const std::string& filename){
  // Block the signal, or the filename in the db will go blank
  g_signal_handler_block(db_filename_entry, filename_callback_id);

  gtk_entry_set_text(db_filename_entry, filename.c_str());

  // Resume normal operation
  g_signal_handler_unblock(db_filename_entry, filename_callback_id);
}

void GtkGUI::set_aos(gulong aos){
  gtk_spin_button_set_value(db_offset_sbutton, aos);
}

void GtkGUI::set_ros(gulong ros){
  gtk_spin_button_set_value(db_rowlen_sbutton, ros);
}

void GtkGUI::show_error(const std::string& message){
  GtkWidget* dialog = gtk_message_dialog_new(main_window,
    GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
    message.c_str());

  g_signal_connect(G_OBJECT(dialog), "response", G_CALLBACK(gtk_widget_destroy),
    NULL);

  gtk_widget_show(dialog);
}

void GtkGUI::extract_to_window(long row_limit){
  Result* result = db.query(row_limit);

  /* Create the window */
  GtkWidget* window;
  GtkWidget* tree_view;
  GtkListStore* list_store;
  {
    window = gtk_dialog_new_with_buttons("Extracted Data",
      main_window, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CLOSE,
      GTK_RESPONSE_NONE, NULL);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    tree_view = gtk_tree_view_new();
    gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(tree_view), TRUE);

    GType* gtype_array = new GType[result->columns];

    for (unsigned int i = 0; i < result->columns; i++){
      GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
      gtk_tree_view_insert_column_with_attributes(
        GTK_TREE_VIEW(tree_view), -1,
        db.entry_at(i).name.c_str(), renderer,
        "text", i, NULL);
      gtype_array[i] = G_TYPE_STRING;
    }

    list_store = gtk_list_store_newv(result->columns,
      gtype_array);
    delete [] gtype_array;

    gtk_container_add(GTK_CONTAINER(GTK_DIALOG(window)->vbox),
      scrolled_window);
    gtk_container_add(GTK_CONTAINER(scrolled_window), tree_view);
    g_signal_connect(G_OBJECT(window), "response",
      G_CALLBACK(gtk_widget_destroy), NULL);
  }

  GtkTreeIter tree_iter;

  const char** data = result->next_row();
  while (data){
    gtk_list_store_append (GTK_LIST_STORE(list_store), &tree_iter);

    for (unsigned int col = 0; col < result->columns; col++){
      if (g_utf8_validate(data[col], -1, NULL)){
        gtk_list_store_set(list_store, &tree_iter, col, data[col], -1);
      }

      else {
        gtk_list_store_set(list_store, &tree_iter, col, "[Driller] Invalid Text", -1);
      }
    }
    data = result->next_row();
  }

  delete result;
  gtk_tree_view_set_model(GTK_TREE_VIEW(tree_view),
    GTK_TREE_MODEL(list_store));
  gtk_widget_show_all(window);
}

void GtkGUI::reload_settings(){
  set_filename(db.filename);
  set_aos(db.aos);
  set_ros(db.ros);

  for (unsigned int i = 0; i < db.entry_count(); i++){
    append_entry(db.entry_at(i));
  }
}

void GtkGUI::init(){
  /* Main window */
  main_window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_window_set_title(GTK_WINDOW(main_window), "Driller");
  gtk_window_set_default_size(GTK_WINDOW(main_window), 400, 300);
  GtkWidget* main_vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(main_window), main_vbox);

  g_signal_connect(G_OBJECT(main_window), "delete_event",
    G_CALLBACK(on_main_window_delete_event), NULL);

  /* Toolbar */
  {
    GtkToolbar* toolbar = GTK_TOOLBAR(gtk_toolbar_new());
    gtk_box_pack_start(GTK_BOX(main_vbox), GTK_WIDGET(toolbar), FALSE, FALSE,0);

    GtkToolItem* toolbar_new = gtk_tool_button_new_from_stock(GTK_STOCK_NEW);
    GtkToolItem* toolbar_open = gtk_tool_button_new_from_stock(GTK_STOCK_OPEN);
    GtkToolItem* toolbar_save = gtk_tool_button_new_from_stock(GTK_STOCK_SAVE);
    GtkToolItem* toolbar_open_dir = gtk_tool_button_new_from_stock(
      GTK_STOCK_FIND);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(toolbar_open_dir),
      "Select Data Directory");
    GtkToolItem* toolbar_extract = gtk_tool_button_new_from_stock(
      GTK_STOCK_CONVERT);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(toolbar_extract), "Extract Data");

    gtk_toolbar_insert(toolbar, toolbar_new, 0);
    gtk_toolbar_insert(toolbar, toolbar_open, 1);
    gtk_toolbar_insert(toolbar, toolbar_save, 2);
    gtk_toolbar_insert(toolbar, toolbar_open_dir, 3);
    gtk_toolbar_insert(toolbar, toolbar_extract, 4);

    g_signal_connect(G_OBJECT(toolbar_new), "clicked", G_CALLBACK(
      on_button_new_clicked), this);
    g_signal_connect(G_OBJECT(toolbar_open), "clicked", G_CALLBACK(
      on_button_open_clicked), this);
    g_signal_connect(G_OBJECT(toolbar_save), "clicked", G_CALLBACK(
      on_button_save_clicked), this);
    g_signal_connect(G_OBJECT(toolbar_open_dir), "clicked", G_CALLBACK(
      on_button_open_dir_clicked), this);
    g_signal_connect(G_OBJECT(toolbar_extract), "clicked", G_CALLBACK(
      on_button_extract_clicked), this);
  }

  /* Database properties */
  {
    GtkWidget* db_expander = gtk_expander_new("<b>Database</b>");
    gtk_expander_set_use_markup(GTK_EXPANDER(db_expander), TRUE);
    gtk_box_pack_start(GTK_BOX(main_vbox), db_expander, FALSE, FALSE, 0);

    GtkWidget* db_alignment = gtk_alignment_new(0.5, 0.5, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(db_alignment), 6, 12, 12, 12);
    gtk_container_add(GTK_CONTAINER(db_expander), db_alignment);

    GtkWidget* db_table = gtk_table_new(3, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(db_alignment), db_table);

    GtkWidget* db_filename_label = gtk_label_new("Database Filename: ");
    GtkWidget* db_offset_label = gtk_label_new("Absolute Offset: ");
    GtkWidget* db_rowlen_label = gtk_label_new("Row Length: ");

    gtk_misc_set_alignment(GTK_MISC(db_filename_label), 0, 0.5);
    gtk_misc_set_alignment(GTK_MISC(db_offset_label), 0, 0.5);
    gtk_misc_set_alignment(GTK_MISC(db_rowlen_label), 0, 0.5);

    gtk_table_attach_defaults(GTK_TABLE(db_table), db_filename_label,
      0, 1, 0, 1);
    gtk_table_attach_defaults(GTK_TABLE(db_table), db_offset_label,
      0, 1, 1, 2);
    gtk_table_attach_defaults(GTK_TABLE(db_table), db_rowlen_label,
      0, 1, 2, 3);

    db_filename_entry = GTK_ENTRY(gtk_entry_new());
    db_offset_sbutton = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0,
      10000000, 1));
    db_rowlen_sbutton = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(1,
      10000000, 1));

    gtk_table_attach_defaults(GTK_TABLE(db_table),
      GTK_WIDGET(db_filename_entry), 1, 2, 0, 1);
    gtk_table_attach_defaults(GTK_TABLE(db_table),
      GTK_WIDGET(db_offset_sbutton), 1, 2, 1, 2);
    gtk_table_attach_defaults(GTK_TABLE(db_table),
      GTK_WIDGET(db_rowlen_sbutton), 1, 2, 2, 3);

    filename_callback_id = g_signal_connect(G_OBJECT(db_filename_entry),
      "changed", G_CALLBACK(on_db_filename_entry_changed), this);

    g_signal_connect(G_OBJECT(db_offset_sbutton), "value-changed",
      G_CALLBACK(on_db_offset_sbutton_value_changed), this);

    g_signal_connect(G_OBJECT(db_rowlen_sbutton), "value-changed",
      G_CALLBACK(on_db_rowlen_sbutton_value_changed), this);
  }

  /* Entries */
  {
    GtkWidget* entry_expander = gtk_expander_new("<b>Entries</b>");
    gtk_expander_set_use_markup(GTK_EXPANDER(entry_expander), TRUE);
    gtk_box_pack_start(GTK_BOX(main_vbox), entry_expander, TRUE, TRUE, 0);

    GtkWidget* entry_alignment = gtk_alignment_new(0.5, 0.5, 1.0, 1.0);
    gtk_alignment_set_padding(GTK_ALIGNMENT(entry_alignment), 6, 12, 12, 12);
    gtk_container_add(GTK_CONTAINER(entry_expander), entry_alignment);

    GtkWidget* entry_vbox = gtk_vbox_new(FALSE, 6);
    gtk_container_add(GTK_CONTAINER(entry_alignment), entry_vbox);

    /* Manipulation buttons */
    GtkWidget* button_hbox = gtk_hbox_new(FALSE, 6);
    gtk_box_pack_start(GTK_BOX(entry_vbox), button_hbox, FALSE, FALSE, 0);

    add_entry_button = GTK_BUTTON(gtk_button_new_from_stock(GTK_STOCK_ADD));
    remove_entry_button = GTK_BUTTON(gtk_button_new_from_stock(
      GTK_STOCK_REMOVE));
    move_entry_up_button = GTK_BUTTON(gtk_button_new_from_stock(
      GTK_STOCK_GO_UP));
    move_entry_down_button = GTK_BUTTON(gtk_button_new_from_stock(
      GTK_STOCK_GO_DOWN));

    gtk_box_pack_start(GTK_BOX(button_hbox), GTK_WIDGET(add_entry_button),
      FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_hbox), GTK_WIDGET(remove_entry_button),
      FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_hbox), GTK_WIDGET(move_entry_up_button),
      FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(button_hbox), GTK_WIDGET(move_entry_down_button),
      FALSE, FALSE, 0);

    gtk_widget_set_sensitive(GTK_WIDGET(remove_entry_button), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(move_entry_up_button), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(move_entry_down_button), FALSE);

    g_signal_connect(G_OBJECT(add_entry_button), "clicked",
      G_CALLBACK(on_button_add_entry_clicked), this);
    g_signal_connect(G_OBJECT(remove_entry_button), "clicked",
      G_CALLBACK(on_button_remove_entry_clicked), this);
    g_signal_connect(G_OBJECT(move_entry_up_button), "clicked",
      G_CALLBACK(on_button_move_entry_up_clicked), this);
    g_signal_connect(G_OBJECT(move_entry_down_button), "clicked",
      G_CALLBACK(on_button_move_entry_down_clicked), this);

    /* Stores the GtkTreeModel for combo boxes in the tree view */
    GtkListStore* combo_model = gtk_list_store_new( 1, G_TYPE_STRING);

    GtkTreeIter iter;
    for (int i = 0; i < ENTRY_NUM_TYPES; i++){
      gtk_list_store_append(combo_model, &iter);
      gtk_list_store_set(combo_model, &iter, 0, entry_type_strings[i].c_str(), -1);
    }

    /* Tree view */
    GtkWidget* scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(entry_vbox), scrolled_window, TRUE, TRUE,
      0);

    entry_view = GTK_TREE_VIEW(gtk_tree_view_new());
    gtk_tree_view_set_rules_hint(entry_view, TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(entry_view));

    /* Create columns */
    GtkListStore* list_store;
    GtkCellRenderer* renderer;
    GtkTreeViewColumn* column;

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(entry_view, -1, "Name",
      renderer, "text", 0, NULL);
    g_object_set(G_OBJECT(renderer), "editable", TRUE, NULL);
    g_object_set_data(G_OBJECT(renderer), "column", (void*)(0));
    g_signal_connect(G_OBJECT(renderer),
      "edited", G_CALLBACK(on_renderer_edited), this);

    renderer = gtk_cell_renderer_combo_new();
    g_object_set(G_OBJECT(renderer), "model", GTK_TREE_MODEL(combo_model), NULL);
    g_object_set(G_OBJECT(renderer), "editable", TRUE, NULL);
    g_object_set(G_OBJECT(renderer), "text-column", 0, NULL);
    column = gtk_tree_view_column_new_with_attributes("Type", renderer,
      "text", 1,  NULL);
    gtk_tree_view_column_set_expand(column, TRUE);
    gtk_tree_view_append_column(entry_view, column);
    g_object_unref(G_OBJECT(combo_model));
    g_object_set_data(G_OBJECT(renderer), "column", (void*)(1));
    g_signal_connect(G_OBJECT(renderer),
      "edited", G_CALLBACK(on_renderer_edited), this);

    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer), "editable", TRUE, NULL);
    gtk_tree_view_insert_column_with_attributes(entry_view, -1, "Offset",
      renderer, "text", 2, NULL);
    g_object_set_data(G_OBJECT(renderer), "column", (void*)(2));
    g_signal_connect(G_OBJECT(renderer),
      "edited", G_CALLBACK(on_renderer_edited), this);

    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer), "editable", TRUE, NULL);
    gtk_tree_view_insert_column_with_attributes(entry_view, -1, "Length",
      renderer, "text", 3, NULL);
    g_object_set_data(G_OBJECT(renderer), "column", (void*)(3));
    g_signal_connect(G_OBJECT(renderer),
      "edited", G_CALLBACK(on_renderer_edited), this);

    renderer = gtk_cell_renderer_toggle_new();
    gtk_tree_view_insert_column_with_attributes(entry_view, -1, "Indexed",
      renderer, "active", 4, NULL);
    g_signal_connect(G_OBJECT(renderer),
      "toggled", G_CALLBACK(on_renderer_toggled), this);

    list_store = gtk_list_store_new( 5,
      G_TYPE_STRING, G_TYPE_STRING, G_TYPE_ULONG, G_TYPE_ULONG,
      G_TYPE_BOOLEAN);
    gtk_tree_view_set_model(entry_view, GTK_TREE_MODEL(list_store));

    g_object_unref(G_OBJECT(list_store));

    g_signal_connect(G_OBJECT(gtk_tree_view_get_selection(entry_view)),
      "changed", G_CALLBACK(on_entry_view_selection_changed), this);
  }

  /* File dialogs */
  {
    open_file_dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Open File",
      GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_OPEN,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      GTK_STOCK_OPEN, GTK_RESPONSE_OK,
      NULL));

    save_file_dialog = GTK_DIALOG(gtk_file_chooser_dialog_new("Save File",
      GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_SAVE,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      GTK_STOCK_SAVE, GTK_RESPONSE_OK,
      NULL));

    open_dir_dialog = GTK_DIALOG(gtk_file_chooser_dialog_new(
      "Select Data Directory",
      GTK_WINDOW(main_window), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      GTK_STOCK_OPEN, GTK_RESPONSE_OK,
      NULL));

    if (g_path_is_absolute(db.path.c_str())){
      gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(open_dir_dialog),
        db.path.c_str());
    }

    /* Add shortcuts */
    char* data_dir = br_find_data_dir("../share");
    char* db_dir = g_build_filename(data_dir, "/driller/Databases/", NULL);

    if (g_path_is_absolute(db_dir)){
      gtk_file_chooser_add_shortcut_folder(GTK_FILE_CHOOSER(open_file_dialog),
        db_dir, NULL);
      gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(open_file_dialog),
        db_dir);
    }
    g_free(data_dir);
    g_free(db_dir);

    /* Add filter */
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Database Descriptions");
    gtk_file_filter_add_pattern(filter, "*.xml");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(open_file_dialog), filter);
  }

  /* Extraction dialog */
  {
    extraction_dialog = GTK_DIALOG(gtk_dialog_new_with_buttons("Extract Data",
      GTK_WINDOW(main_window), GTK_DIALOG_DESTROY_WITH_PARENT,
      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
      GTK_STOCK_OK, GTK_RESPONSE_OK,
      NULL));

    GtkWidget* output_frame = gtk_frame_new("<b>Extraction Output</b>");
    gtk_label_set_use_markup(GTK_LABEL(gtk_frame_get_label_widget(
      GTK_FRAME(output_frame))), TRUE);
    gtk_frame_set_shadow_type(GTK_FRAME(output_frame), GTK_SHADOW_NONE);
    gtk_box_pack_start(GTK_BOX(extraction_dialog->vbox), output_frame, FALSE,
      FALSE, 0);
    GtkWidget* output_vbox = gtk_vbox_new(FALSE, 0);
    gtk_container_add(GTK_CONTAINER(output_frame), output_vbox);

    /* Number of rows to extract */
    GtkWidget* rows_hbox = gtk_hbox_new(FALSE, 12);
    gtk_box_pack_start(GTK_BOX(output_vbox), rows_hbox,
      FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(rows_hbox), gtk_label_new("How many rows:"),
      FALSE, FALSE, 0);

    row_count_sbutton = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(1,
      10000000, 1));
    gtk_box_pack_start(GTK_BOX(rows_hbox), GTK_WIDGET(row_count_sbutton),
      FALSE, FALSE, 0);

    extract_all_rows_cbutton = GTK_CHECK_BUTTON(gtk_check_button_new_with_label(
      "Extract all rows"));
    gtk_box_pack_start(GTK_BOX(rows_hbox), GTK_WIDGET(extract_all_rows_cbutton),
      FALSE, FALSE, 0);

    /* Output selection */
    output_window_radio = GTK_RADIO_BUTTON(gtk_radio_button_new_with_label(NULL,
      "Temporary Window"));
    output_text_radio = GTK_RADIO_BUTTON(
      gtk_radio_button_new_with_label_from_widget(output_window_radio,
      "Text File"));

    gtk_box_pack_start(GTK_BOX(output_vbox), GTK_WIDGET(output_window_radio),
      FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(output_vbox), GTK_WIDGET(output_text_radio),
      FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(output_window_radio), "clicked",
      G_CALLBACK(on_radio_button_clicked), this);
    g_signal_connect(G_OBJECT(output_text_radio), "clicked",
      G_CALLBACK(on_radio_button_clicked), this);

#if ENABLE_MYSQL
    output_mysql_radio = GTK_RADIO_BUTTON(
      gtk_radio_button_new_with_label_from_widget(output_window_radio,
      "MySQL Database"));

    gtk_box_pack_start(GTK_BOX(output_vbox), GTK_WIDGET(output_mysql_radio),
      FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(output_mysql_radio), "clicked",
      G_CALLBACK(on_radio_button_clicked), this);
#endif

#if ENABLE_POSTGRESQL
    output_postgresql_radio = GTK_RADIO_BUTTON(
      gtk_radio_button_new_with_label_from_widget(output_window_radio,
      "PostgreSQL Database"));

    gtk_box_pack_start(GTK_BOX(output_vbox), GTK_WIDGET(output_postgresql_radio),
      FALSE, FALSE, 0);

    g_signal_connect(G_OBJECT(output_postgresql_radio), "clicked",
      G_CALLBACK(on_radio_button_clicked), this);
#endif

    /* Text specific options */
    text_frame = GTK_FRAME(gtk_frame_new("<b>Text options</b>"));
    gtk_label_set_use_markup(GTK_LABEL(
      gtk_frame_get_label_widget(text_frame)), TRUE);
    gtk_frame_set_shadow_type(text_frame, GTK_SHADOW_NONE);
    gtk_widget_hide(GTK_WIDGET(text_frame));
    gtk_box_pack_start(GTK_BOX(extraction_dialog->vbox),
      GTK_WIDGET(text_frame), FALSE, FALSE, 0);

    GtkWidget* text_hbox = gtk_hbox_new(FALSE, 6);

    gtk_container_add(GTK_CONTAINER(text_frame), text_hbox);

    GtkWidget* text_label = gtk_label_new("File name:");
    output_text_filename = GTK_ENTRY(gtk_entry_new());

    gtk_box_pack_start(GTK_BOX(text_hbox),
      text_label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(text_hbox),
      GTK_WIDGET(output_text_filename), TRUE, TRUE, 0);

#if ENABLE_SQL
    /* SQL specific options */
    sql_frame = GTK_FRAME(gtk_frame_new("<b>SQL options</b>"));
    gtk_label_set_use_markup(GTK_LABEL(
      gtk_frame_get_label_widget(sql_frame)), TRUE);
    gtk_frame_set_shadow_type(sql_frame, GTK_SHADOW_NONE);
    gtk_widget_hide(GTK_WIDGET(sql_frame));
    gtk_box_pack_start(GTK_BOX(extraction_dialog->vbox),
      GTK_WIDGET(sql_frame), FALSE, FALSE, 0);
    GtkWidget* sql_table = gtk_table_new(6, 2, FALSE);
    gtk_container_add(GTK_CONTAINER(sql_frame), sql_table);

    sql_server_entry = GTK_ENTRY(gtk_entry_new());
    sql_port_sbutton = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(1,
      65535, 1));
    sql_username_entry = GTK_ENTRY(gtk_entry_new());
    sql_password_entry = GTK_ENTRY(gtk_entry_new());
    sql_database_entry = GTK_ENTRY(gtk_entry_new());
    sql_table_entry = GTK_ENTRY(gtk_entry_new());

    gtk_spin_button_set_value(sql_port_sbutton, 3306);
    gtk_entry_set_visibility(sql_password_entry, FALSE);
    gtk_entry_set_invisible_char(sql_password_entry, '*');

    GtkWidget* sql_server_label = gtk_label_new("Server address:");
    GtkWidget* sql_port_label = gtk_label_new("Server port:");
    GtkWidget* sql_username_label = gtk_label_new("Username:");
    GtkWidget* sql_password_label = gtk_label_new("Password:");
    GtkWidget* sql_database_label = gtk_label_new("Database name:");
    GtkWidget* sql_table_label = gtk_label_new("Desired table name:");

    gtk_misc_set_alignment(GTK_MISC(sql_server_label), 0, 0.5);
    gtk_misc_set_alignment(GTK_MISC(sql_port_label), 0, 0.5);
    gtk_misc_set_alignment(GTK_MISC(sql_username_label), 0, 0.5);
    gtk_misc_set_alignment(GTK_MISC(sql_password_label), 0, 0.5);
    gtk_misc_set_alignment(GTK_MISC(sql_database_label), 0, 0.5);
    gtk_misc_set_alignment(GTK_MISC(sql_table_label), 0, 0.5);

    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      sql_server_label, 0, 1, 0, 1);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      sql_port_label, 0, 1, 1, 2);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      sql_username_label, 0, 1, 2, 3);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      sql_password_label, 0, 1, 3, 4);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      sql_database_label, 0, 1, 4, 5);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      sql_table_label, 0, 1, 5, 6);

    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      GTK_WIDGET(sql_server_entry), 1, 2, 0, 1);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      GTK_WIDGET(sql_port_sbutton), 1, 2, 1, 2);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      GTK_WIDGET(sql_username_entry), 1, 2, 2, 3);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      GTK_WIDGET(sql_password_entry), 1, 2, 3, 4);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      GTK_WIDGET(sql_database_entry), 1, 2, 4, 5);
    gtk_table_attach_defaults(GTK_TABLE(sql_table),
      GTK_WIDGET(sql_table_entry), 1, 2, 5, 6);
#endif

    gtk_widget_show_all(output_frame);
  }

  gtk_widget_show_all(GTK_WIDGET(main_window));
}

/** SIGNAL HANDLERS **/

gboolean GtkGUI::on_main_window_delete_event(GtkWidget* widget,
  GdkEvent* event, gpointer data){

  gtk_main_quit();
  return FALSE;
}

void GtkGUI::on_button_new_clicked(GtkWidget* widget, gpointer data){
  GtkGUI* gui = (GtkGUI*)(data);

  GtkListStore* list_store = GTK_LIST_STORE(gtk_tree_view_get_model(
    gui->entry_view));

  gui->db.clear();
  gtk_list_store_clear(list_store);
  gtk_entry_set_text(gui->db_filename_entry, "");
  gtk_spin_button_set_value(gui->db_offset_sbutton, 0);
  gtk_spin_button_set_value(gui->db_rowlen_sbutton, 1);

  gtk_widget_set_sensitive(GTK_WIDGET(gui->remove_entry_button), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_up_button), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_down_button), FALSE);
}

void GtkGUI::on_button_open_clicked(GtkWidget* widget, gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);

  gint response = gtk_dialog_run(GTK_DIALOG(gui->open_file_dialog));
  gtk_widget_hide(GTK_WIDGET(gui->open_file_dialog));

  if (response == GTK_RESPONSE_OK){
    char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(
      gui->open_file_dialog));
    gtk_list_store_clear(GTK_LIST_STORE(gtk_tree_view_get_model(
      gui->entry_view)));
    try {
      gui->app.parse_description_file(filename);
    }

    catch (const std::exception& e){
      gui->show_error(e.what());
    }
    g_free(filename);
  }

  gtk_widget_set_sensitive(GTK_WIDGET(gui->remove_entry_button), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_up_button), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_down_button), FALSE);
}

void GtkGUI::on_button_save_clicked(GtkWidget* widget, gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);

  gint response = gtk_dialog_run(GTK_DIALOG(gui->save_file_dialog));
  gtk_widget_hide(GTK_WIDGET(gui->save_file_dialog));

  if (response == GTK_RESPONSE_OK){
    char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(
      gui->save_file_dialog));
    try {
      gui->app.save_description_file(filename);
    }

    catch (const std::exception& e){
      gui->show_error(e.what());
    }
    g_free(filename);
  }
}

void GtkGUI::on_button_open_dir_clicked(GtkWidget* widget, gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);

  gint response = gtk_dialog_run(GTK_DIALOG(gui->open_dir_dialog));
  gtk_widget_hide(GTK_WIDGET(gui->open_dir_dialog));

  if (response == GTK_RESPONSE_OK){
    char* path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(
      gui->open_dir_dialog));
    try {
      gui->db.path = path;
    }

    catch (const std::exception& e){
      gui->show_error(e.what());
    }
    g_free(path);
  }
}

void GtkGUI::on_button_extract_clicked(GtkWidget* widget, gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);

  gint response = gtk_dialog_run(GTK_DIALOG(gui->extraction_dialog));
  gtk_widget_hide(GTK_WIDGET(gui->extraction_dialog));

  int rows = gtk_spin_button_get_value_as_int(gui->row_count_sbutton);
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(
    gui->extract_all_rows_cbutton))){

    rows = -1;
  }

  try {

  if (response == GTK_RESPONSE_OK){
    switch(gui->output){
      /* Temporary window */
      case 0:
        gui->extract_to_window(rows);
        break;

      /* File */
      case 1: {
        const char* filename = gtk_entry_get_text(gui->output_text_filename);
        gui->app.extract_to_file(filename, rows);
        break;
      }

      /* MySQL */
      case 2: {
        #if ENABLE_MYSQL
          char* password = g_strdup("");
          try {
            const char* server = gtk_entry_get_text(gui->sql_server_entry);
            int port = gtk_spin_button_get_value_as_int(gui->sql_port_sbutton);
            const char* username = gtk_entry_get_text(gui->sql_username_entry);
            password = g_strdup(gtk_entry_get_text(gui->sql_password_entry));
            const char* db = gtk_entry_get_text(gui->sql_database_entry);
            const char* table = gtk_entry_get_text(gui->sql_table_entry);

            gtk_entry_set_text(gui->sql_password_entry, "");
            gui->app.extract_to_mysql(server, port, username, password,
              db, table, rows);
            free(password);
          } catch (const std::exception& e){
            free(password);
            throw;
          }
        #endif
        break;
      }

      /* PostgreSQL */
      case 3: {
        #if ENABLE_POSTGRESQL
          char* password = g_strdup("");
          try {
            const char* server = gtk_entry_get_text(gui->sql_server_entry);
            int port = gtk_spin_button_get_value_as_int(gui->sql_port_sbutton);
            const char* username = gtk_entry_get_text(gui->sql_username_entry);
            password = g_strdup(gtk_entry_get_text(gui->sql_password_entry));
            const char* db = gtk_entry_get_text(gui->sql_database_entry);
            const char* table = gtk_entry_get_text(gui->sql_table_entry);

            gtk_entry_set_text(gui->sql_password_entry, "");
            gui->app.extract_to_postgresql(server, port, username, password,
              db, table, rows);
            free(password);
          } catch (const std::exception& e){
            free(password);
            throw;
          }
        #endif
        break;
      }
    }
  }

  }

  catch (const std::exception& e){
    gui->show_error(e.what());
  }
}

void GtkGUI::on_button_add_entry_clicked(GtkWidget* widget,
  gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);
  Entry entry;
  gui->db.append_entry(entry);
  gui->append_entry(entry);
  gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_down_button), TRUE);
}

void GtkGUI::on_button_remove_entry_clicked(GtkWidget* widget,
  gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);

  GtkTreeSelection* selection = gtk_tree_view_get_selection(gui->entry_view);
  GtkTreeModel* model = gtk_tree_view_get_model(gui->entry_view);
  GtkTreeIter iter;
  guint index;

  if (gtk_tree_selection_get_selected(selection, NULL, &iter)){
    /* Remove the entry from the database */
    GtkTreePath* path = gtk_tree_model_get_path(model, &iter);
    index = gtk_tree_path_get_indices(path)[0];
    gui->db.remove_entry(index);
    gtk_tree_path_free(path);

    /* Remove the selected row */
    if (gtk_list_store_remove(GTK_LIST_STORE(model), &iter)){
      /* Select the next row */
      gtk_tree_selection_select_iter(selection, &iter);
    }
  }

  else {
    return;
  }

  if (index >= gui->db.entry_count()){
    gtk_widget_set_sensitive(GTK_WIDGET(gui->remove_entry_button), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_up_button), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_down_button), FALSE);
  }
}

void GtkGUI::on_button_move_entry_up_clicked(GtkWidget* widget,
  gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);
  GtkListStore* list_store = (GtkListStore*)(gtk_tree_view_get_model(
    gui->entry_view));
  GtkTreeSelection* selection = gtk_tree_view_get_selection(gui->entry_view);
  GtkTreeIter iter, pos;
  GtkTreePath* path;

  if (!gtk_tree_selection_get_selected(selection, NULL, &iter)){
    return;
  }

  path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_store), &iter);
  guint index = gtk_tree_path_get_indices(path)[0];

  if (index == 0) return;

  gtk_tree_path_prev(path);
  gtk_tree_model_get_iter(GTK_TREE_MODEL(list_store), &pos, path);
  gtk_list_store_move_before(list_store, &iter, &pos);
  gtk_tree_path_free(path);

  /* Move database entries */
  Entry entry = gui->db.entry_at(index);

  gui->db.remove_entry(index);
  gui->db.insert_entry(entry, index-1);

  gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_down_button), TRUE);

  if (index == 1){
    gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_up_button), FALSE);
  }
}

void GtkGUI::on_button_move_entry_down_clicked(GtkWidget* widget,
  gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);
  GtkListStore* list_store = (GtkListStore*)(gtk_tree_view_get_model(
    gui->entry_view));
  GtkTreeSelection* selection = gtk_tree_view_get_selection(gui->entry_view);
  GtkTreeIter iter, pos;
  GtkTreePath* path;

  if (!gtk_tree_selection_get_selected(selection, NULL, &iter)){
    return;
  }

  path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_store), &iter);
  guint index = gtk_tree_path_get_indices(path)[0];

  if (index >= gui->db.entry_count()-1) return;

  gtk_tree_path_next(path);
  gtk_tree_model_get_iter(GTK_TREE_MODEL(list_store), &pos, path);
  gtk_list_store_move_after(list_store, &iter, &pos);
  gtk_tree_path_free(path);

  /* Move database entries */
  Entry entry = gui->db.entry_at(index);

  gui->db.remove_entry(index);
  gui->db.insert_entry(entry, index+1);

  gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_up_button), TRUE);

  if (index+2 >= gui->db.entry_count()){
    gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_down_button), FALSE);
  }
}

void GtkGUI::on_renderer_edited(GtkCellRendererText* renderer, gchar* arga,
  gchar* argb, gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);
  GtkListStore* list_store = (GtkListStore*)(gtk_tree_view_get_model(
    gui->entry_view));
  GtkTreeIter iter;
  gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(list_store), &iter, arga);

  int column = reinterpret_cast<int>(g_object_get_data(G_OBJECT(renderer), "column"));
  int row;
  sscanf(arga, "%d", &row);

  switch(column){
    /* Name */
    case 0:
      gui->db.entry_at(row).name = argb;
      gtk_list_store_set(list_store, &iter, column, argb, -1);
      break;

    /* Type */
    case 1:
      gui->db.entry_at(row).set_type_from_string(argb);
      gtk_list_store_set(list_store, &iter, column, argb, -1);
      break;

    /* Offset */
    case 2:{
      unsigned long offset;
      sscanf(argb, "%lu", &offset);
      gui->db.entry_at(row).offset = offset;
      gtk_list_store_set(list_store, &iter, column, offset, -1);
      break;
    }

    /* Length */
    case 3:{
      unsigned long length;
      sscanf(argb, "%lu", &length);
      gui->db.entry_at(row).length = length;
      gtk_list_store_set(list_store, &iter, column, length, -1);
      break;
    }
  }
}

void GtkGUI::on_renderer_toggled(GtkCellRendererToggle* renderer, gchar* path,
  gpointer* data){

  GtkGUI* gui = (GtkGUI*)(data);
  int row;
  sscanf(path, "%d", &row);

  GtkListStore* list_store = (GtkListStore*)(gtk_tree_view_get_model(
    gui->entry_view));
  GtkTreeIter iter;
  gtk_tree_model_get_iter_from_string(GTK_TREE_MODEL(list_store), &iter, path);

  gboolean indexed;
  gtk_tree_model_get(GTK_TREE_MODEL(list_store), &iter, 4, &indexed, -1);
  indexed = !indexed;

  gui->db.entry_at(row).indexed = static_cast<bool>(indexed);
  gtk_list_store_set(list_store, &iter, 4, indexed, -1);
}

void GtkGUI::on_db_filename_entry_changed(GtkWidget* widget, gpointer data){
  GtkGUI* gui = (GtkGUI*)(data);

  gui->db.filename = gtk_entry_get_text(GTK_ENTRY(widget));
}

void GtkGUI::on_db_offset_sbutton_value_changed(GtkWidget* widget, gpointer data){
  GtkGUI* gui = (GtkGUI*)(data);

  gui->db.aos = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
}

void GtkGUI::on_db_rowlen_sbutton_value_changed(GtkWidget* widget, gpointer data){
  GtkGUI* gui = (GtkGUI*)(data);

  gui->db.ros = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget));
}

void GtkGUI::on_entry_view_selection_changed(
  GtkTreeSelection* selection, gpointer data){

  GtkGUI* gui = (GtkGUI*)(data);

  gtk_widget_set_sensitive(GTK_WIDGET(gui->remove_entry_button), TRUE);

  GtkTreeIter iter;

  if (!gtk_tree_selection_get_selected(selection, NULL, &iter)){
    return;
  }

  GtkListStore* list_store = (GtkListStore*)(gtk_tree_view_get_model(
    gui->entry_view));
  GtkTreePath* path;
  path = gtk_tree_model_get_path(GTK_TREE_MODEL(list_store), &iter);
  guint index = gtk_tree_path_get_indices(path)[0];

  if (index > 0){
    gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_up_button), TRUE);
  }

  else {
    gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_up_button), FALSE);
  }

  if (index < gui->db.entry_count()-1){
    gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_down_button), TRUE);
  }

  else {
    gtk_widget_set_sensitive(GTK_WIDGET(gui->move_entry_down_button), FALSE);
  }

}

void GtkGUI::on_radio_button_clicked(GtkWidget* widget, gpointer data){
  GtkGUI* gui = (GtkGUI*)(data);

  if (widget == GTK_WIDGET(gui->output_mysql_radio) ||
      widget == GTK_WIDGET(gui->output_postgresql_radio)){
    gtk_widget_show_all(GTK_WIDGET(gui->sql_frame));
    gui->output = 2;
  }

  else {
    gtk_widget_hide(GTK_WIDGET(gui->sql_frame));
  }

  if (widget == GTK_WIDGET(gui->output_text_radio)){
    gtk_widget_show_all(GTK_WIDGET(gui->text_frame));
    gui->output = 1;
  }

  else {
    gtk_widget_hide(GTK_WIDGET(gui->text_frame));
  }

  if (widget == GTK_WIDGET(gui->output_window_radio)){
    gui->output = 0;
  }
}

} // namespace
