#pragma once
#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include <unistd.h>

//A number of misc functions that are used in multiple places
class Functions
{
	public:
		static std::string get_OS();
		static std::string get_first_folder(std::string folder);
		static std::string get_folder_for_filename(std::string filename);
		static std::string instance_version_minus_v(std::string instance_version);
		static std::string get_home_dir();

		static std::vector<std::string> get_files_in_folder(std::string folder, std::string extension = "");

		static int get_number_of_files_in_folder(std::string folder);

		static bool exists_modplugins(std::string datadir);
		static bool is_plugin_installed(const std::string &plugin_name);
		static bool str_replace(std::string &str, const std::string &from, const std::string &to);
		static bool has_v(std::string instance_version);

		static void open_folder(std::string instance_name, std::string instance_type, std::string instance_version);
		static void launch_game(const std::string &instance_name, const std::string &instance_type, const std::string &instance_version, bool untouched);
		static void open_data_folder();

	private:
};