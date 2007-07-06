/* Driller, a data extraction program
 * Copyright (C) 2005-2006 John Millikin
 *
 * driller.cpp
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

#include "driller.h"
#include "binreloc.h"
#include <iostream>
#include <fstream>
#include "file_sink.h"
#include "gui.h"
#include <errno.h>

#ifdef CONFIG_H
#include <config.h>
#endif

#if ENABLE_MYSQL
#include "mysql_sink.h"
#endif

using namespace Driller;

// Global settings for connecting to MySQL, if needed
std::string mysql_host = "localhost",
  mysql_username,
  mysql_password,
  mysql_database;
unsigned int mysql_port;

// Database schema files to extract
std::vector<std::string> files;

#ifdef WIN32
  #include <windows.h>
#endif

#ifdef __APPLE__
  #define __unix
#endif
std::string settings_file_name(){
  #ifdef WIN32
    char* APPDATA = getenv("APPDATA");
    if (APPDATA){
      return std::string(APPDATA) + "/.driller.ini";
    }
    else {
      // If APPDATA is NULL, don't save a file
      throw Errors::GenericError("APPDATA doesn't exist. Can't save settings!");
    }
  #endif

  #ifdef __unix
    char* HOME = getenv("HOME");
    if (HOME){
      return std::string(HOME) + "/.driller.ini";
    }
    else {
      // User has no home directory!?
      throw Errors::GenericError(
        "You have no home directory. Can't save settings!"
      );
    }
  #endif
}

std::string find_dentrix_path(){
  std::string path = ".";

  // Try to read a saved path from a file
  std::string filename = settings_file_name();
  std::ifstream file(filename.c_str());

  if (file.is_open()){
    std::string temp;

    // Ignore the first line
    getline(file, temp);

    // Read in the "data_path ="
    file >> temp >> temp;

    // Ignore the space
    file.ignore(1, ' ');

    // And the path
    getline(file, path);

    file.close();

    return path;
  }

  // Couldn't find a saved settings file

#ifdef WIN32
  // Try to read the Registry for Dentrix's data location
  DWORD lpcbData;
  HKEY key;
  LONG retval = RegOpenKeyEx(HKEY_CURRENT_USER,
    reinterpret_cast<LPCTSTR>(
      "Software\\Dentrix Dental Systems, Inc.\\Dentrix\\General"
    ), 0, KEY_READ, &key);

  // Error, call has succeeded?
  if (retval == ERROR_SUCCESS) {
    RegQueryValueEx(key, reinterpret_cast<LPCTSTR>("Path"), NULL, NULL, NULL,
      &lpcbData);

    char* win32_path = new char[lpcbData];

    retval = RegQueryValueEx(key, reinterpret_cast<LPCTSTR>("Path"), NULL, NULL,
      (LPBYTE)(win32_path), &lpcbData);

    if (retval == ERROR_SUCCESS){
      path = win32_path;
    }

    else {
      path = ".";
    }

    delete [] win32_path;
  }
#endif

  return path;
}

void save_dentrix_path(){
  std::string filename = settings_file_name();
  std::ofstream file(filename.c_str());

  if (!file.is_open()){
    throw Errors::FileWriteError(filename, errno);
  }

  file << "[Driller]\n";
  file << "data_path = " << Database::get_data_path() << "\n";

  file.close();
}

void parse_option(std::string option){
  if (option.substr(0, 2) == "--"){
    option.erase(0, 2);

    // This is an option
    std::string key, value;

    int split_index = static_cast<int>(option.find("="));

    if (split_index < 0){
      return;
    }

    key = option.substr(0, split_index);
    value = option.substr(split_index + 1);

    if (key == "host"){
      mysql_host = value;
    }

    else if (key == "username"){
      mysql_username = value;
    }

    else if (key == "password"){
      mysql_password = value;
    }

    else if (key == "database"){
      mysql_database = value;
    }

    else if (key == "port"){
      char* unused;
      mysql_port = strtoul(value.c_str(), &unused, 10);
    }

    else {
      std::cerr << "WARNING: unknown option '" << key << "'\n";
    }
  }

  else {
    // This is a file name
    files.push_back(option);
  }
}

int run_text_version(int argc, char** argv){
  // Parse commandline arguments
  for (int i = 1; i < argc; i++){
    parse_option(argv[i]);
  }

#if ENABLE_MYSQL
  if (files.size() > 0){
    // Construct a MySQL data sink
    MySQLSink sink(mysql_host,
      mysql_username,
      mysql_password,
      mysql_database,
      mysql_port);

    for (unsigned int i = 0; i < files.size(); i++){
      sink.output_database(Database::from_file(files.at(i)));
    }
  }
#endif

  return 0;
}

int main(int argc, char** argv){
  // Enable relocation of the binary
  br_init(NULL);

  Database::set_data_path(find_dentrix_path());

  int return_code;
  if (argc > 1) {
    return_code = run_text_version(argc, argv);
  }
  else {
#if ENABLE_GUI
    GUI gui(argc, argv);
    return_code = gui.run();
#endif
  }

  // Save the data path
  save_dentrix_path();

  return return_code;
}

#ifdef _WINDOWS
int APIENTRY WinMain(HINSTANCE,HINSTANCE,LPSTR,int){
  return main(__argc, __argv);
}
#endif
