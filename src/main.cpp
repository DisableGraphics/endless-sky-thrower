#include <cstddef>
#include <gtkmm.h>

#include <iostream>
#include <thread>
#include <filesystem>
#include <unistd.h>
#include <curl/curl.h>

#include "downloader.hpp"
#include "main_window.hpp"
#include "secondary_dialogs.hpp"

int main(int argc, char* argv[])
{
	try
	{
		auto app = Gtk::Application::create("org.gtkmm.examples.base");
		//Check if a file named "/tmp/esthrower.lock" exists. If it does, then the program is already running.
		//If it doesn't, then create the file and continue.
		if(std::filesystem::exists(std::filesystem::temp_directory_path().string() + "/esthrower.lock"))
		{
			//std::cout << std::filesystem::temp_directory_path().string() + "esthrower.lock" << std::endl;
			InformationDialog dialog("Error", "An instance of ESThrower is already running.\nExiting...", true);
			dialog.run();

			std::cout << "The program is already running. Exiting..." << std::endl;
			return -1;
		}
		else
		{
			std::ofstream lock_file(std::filesystem::temp_directory_path().string() + "/esthrower.lock");
			lock_file.close();
		}
		curl_global_init(CURL_GLOBAL_ALL);
		
		std::cout << "[INFO] Starting ESThrower..." << std::endl;

		if(!std::filesystem::exists(global::config_dir))
		{
			std::filesystem::create_directory(global::config_dir);
		}
		if(!std::filesystem::exists(global::config_dir + "download"))
		{
			std::filesystem::create_directory(global::config_dir + "download");
		}
		MyWindow win;
		
		Gtk::ProgressBar * global_prog = win.get_progress();
		bool connected_to_internet = Downloader::ping();
		if (connected_to_internet)
		{
			std::cout << "[INFO] Connected to the internet!\n";
			Downloader::download_plugin_json();
		}
		std::cout << "[INFO] Checking for instances... ";
		if(std::filesystem::exists(global::config_dir + "download/instances.json"))
		{
			for(auto & p : win.read_instances())
			{
				win.add_instance(p.get_name(), p.get_typee(), p.get_version(), (Gtk::Window*) &win, p.get_autoupdate(), p.get_untouched());
				if(p.get_autoupdate() == true && connected_to_internet)
				{
					p.download();
				}
			}
		}
		std::cout << "Done" << std::endl;
		win.show_all();

		app->run(win);
		curl_global_cleanup();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		std::filesystem::remove(std::filesystem::temp_directory_path().string() + "/esthrower.lock");
		InformationDialog dialog("Error", (std::string)"ESThrower crashed with the following exception: \n" + e.what(), true);
		dialog.run();
		return -1;
	}
	
	return 0;
}
