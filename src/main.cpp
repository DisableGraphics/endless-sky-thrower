#include <cstddef>
#include <gtkmm.h>

#include <iostream>
#include <thread>
#include <filesystem>
#include <unistd.h>
#include <curl/curl.h>

#include "aria.hpp"
#include "gtkmm/progressbar.h"
#include "gtkmm/window.h"
#include "main_window.hpp"

int main(int argc, char* argv[])
{
	#ifdef __linux__
	//Check if a file named "/tmp/esthrower.lock" exists. If it does, then the program is already running.
	//If it doesn't, then create the file and continue.
	if(std::filesystem::exists("/tmp/esthrower.lock"))
	{
		std::cout << "The program is already running. Exiting..." << std::endl;
		return 0;
	}
	else
	{
		std::ofstream lock_file("/tmp/esthrower.lock");
		lock_file.close();
	}
	#endif
  	curl_global_init(CURL_GLOBAL_ALL);
	
	auto app = Gtk::Application::create("org.gtkmm.examples.base");
	MyWindow win;
	download_plugin_json();
	
	if(!std::filesystem::exists("download"))
	{
		std::filesystem::create_directory("download");
	}
	Gtk::ProgressBar * global_prog = win.get_progress();
	Gtk::Window * window = &win;
	for(auto & p : read_instances(global_prog, window))
	{
		win.add_instance(p.get_name(), p.get_typee(), p.get_version(), window, p.get_autoupdate(), p.get_untouched());
		if(p.get_autoupdate() == true)
		{
			p.download();
		}
	}
	win.show_all();

	app->run(win);
	curl_global_cleanup();
	
	return 0;
}
