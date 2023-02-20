#include <cstddef>
#include <gtkmm.h>

#include <iostream>
#include <thread>
#include <filesystem>
#include <unistd.h>
#include <curl/curl.h>

#include "downloader.hpp"
#include "gtkmm/dialog.h"
#include "gtkmm/progressbar.h"
#include "gtkmm/window.h"
#include "main_window.hpp"
#include "secondary_dialogs.hpp"

int main(int argc, char* argv[])
{
	auto app = Gtk::Application::create("org.gtkmm.examples.base");
	#ifdef __linux__
	//Check if a file named "/tmp/esthrower.lock" exists. If it does, then the program is already running.
	//If it doesn't, then create the file and continue.
	if(std::filesystem::exists("/tmp/esthrower.lock"))
	{
		InformationDialog dialog("Error", "An instance of ESThrower is already running.\nExiting...", true);
		dialog.run();

		std::cout << "The program is already running. Exiting..." << std::endl;
		return -1;
	}
	else
	{
		std::ofstream lock_file("/tmp/esthrower.lock");
		lock_file.close();
	}
	#elif _WIN32
	//Put a esthrower.lock file in the temp folder
	if(std::filesystem::exists(std::filesystem::temp_directory_path().string() + "/esthrower.lock"))
	{
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
	#endif
  	curl_global_init(CURL_GLOBAL_ALL);
	
	std::cout << "[INFO] Starting ESThrower..." << std::endl;
	MyWindow win;
	
	
	if(!std::filesystem::exists("download"))
	{
		std::filesystem::create_directory("download");
	}
	download_plugin_json();
	Gtk::ProgressBar * global_prog = win.get_progress();
	Gtk::Window * window = &win;
	std::cout << "Checking for instances... ";
	if(std::filesystem::exists("download/instances.json"))
	{
		for(auto & p : read_instances(global_prog, window))
		{
			win.add_instance(p.get_name(), p.get_typee(), p.get_version(), window, p.get_autoupdate(), p.get_untouched());
			if(p.get_autoupdate() == true)
			{
				p.download();
			}
		}
	}
	std::cout << "Done" << std::endl;
	win.show_all();

	app->run(win);
	curl_global_cleanup();
	
	return 0;
}
