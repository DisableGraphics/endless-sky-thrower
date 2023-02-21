#pragma once
#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include "global_variables.hpp"
#include "gtkmm/dialog.h"
#include "secondary_dialogs.hpp"
#include <thread>
#include <unistd.h>

class Functions
{
	public:
		static std::string get_OS();
		static std::string get_first_folder(std::string folder);
		static std::string get_folder_for_filename(std::string filename);
		static int get_number_of_files_in_folder(std::string folder);
		static bool exists_modplugins(std::string datadir);
		
		static bool is_plugin_installed(const std::string &plugin_name);

		static bool str_replace(std::string &str, const std::string &from, const std::string &to);

		static void open_folder(std::string instance_name, std::string instance_type, std::string instance_version);
		static void launch_game(const std::string &instance_name, const std::string &instance_type, const std::string &instance_version, bool untouched);
	private:
};