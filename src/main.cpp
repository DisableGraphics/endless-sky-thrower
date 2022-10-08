#include <cstddef>
#include <gtkmm.h>

#include <iostream>
#include <thread>
#include <filesystem>
#include <unistd.h>
#include <curl/curl.h>

#include "aria.hpp"
#include "gtkmm/progressbar.h"
#include "main_window.hpp"

int main(int argc, char* argv[])
{
  	curl_global_init(CURL_GLOBAL_ALL);
	
	auto app = Gtk::Application::create("org.gtkmm.examples.base");
	MyWindow win;
	download_plugin_json();
	
	if(!std::filesystem::exists("download"))
	{
		std::filesystem::create_directory("download");
	}
	Gtk::ProgressBar * global_prog = win.get_progress();
	for(auto & p : read_instances(global_prog))
	{
		win.add_instance(p.get_name(), p.get_typee(), p.get_version());
	}
	win.show_all();

	app->run(win);
	curl_global_cleanup();
	
	return 0;
}
