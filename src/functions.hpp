#pragma once
#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include "global_variables.hpp"
#include "aria.hpp"
#include "gtkmm/dialog.h"
#include "main_window.hpp"
#include <thread>
#include <unistd.h>

inline bool is_plugin_installed(const std::string &plugin_name)
{
	// = "/usr/share/aria/plugins/" + plugin_name;
	std::string os = get_OS();
	std::string plugin_folder;

	if(os == "Linux")
	{
		plugin_folder = std::getenv("HOME") + std::string("/.local/share/endless-sky/plugins/") + plugin_name;
	}
	else if(os == "Windows")
	{
		plugin_folder = std::getenv("APPDATA") + std::string("/endless-sky/plugins/") + plugin_name;
	}
	else if(os == "MacOS")
	{
		plugin_folder = std::getenv("HOME") + std::string("/Library/Application Support/endless-sky/plugins/") + plugin_name;
	}

	if (std::filesystem::exists(plugin_folder))
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline std::string get_folder_for_filename(std::string filename)
{
	std::string folder;
	for(int i{static_cast<int>(filename.size() - 1)}; i >= 0; i--)
	{
		if(filename[i] == '/')
		{
			folder = filename.substr(0, i);
			break;
		}
	}
	return folder;
}

inline void open_folder(std::string instance_name, std::string instance_type, std::string instance_version)
{
	if(get_OS() == "Linux")
  	{
		if(instance_type != "Custom")
		{
			std::string command = "xdg-open \"download/" + instance_name + "\"";
			system(command.c_str());
		}
		else
		{
			std::string command = "xdg-open \"" + get_folder_for_filename(instance_version) + "\"";
			
			system(command.c_str());
		}
	}
	else if(get_OS() == "Windows")
	{
		if(instance_type != "Custom")
		{
			std::string command = "explorer \"download/" + instance_name + "\"";
			system(command.c_str());
		}
		else 
		{
			std::string command = "explorer \"" + get_folder_for_filename(instance_version) + "\"";
			system(command.c_str());
		}
	}
	else
	{
		if(instance_type != "Custom")
		{
			std::string command = "open \"download/" + instance_name + "\"";
			system(command.c_str());
		}
		else
		{
			std::string command = "open \"" + get_folder_for_filename(instance_version) + "\"";
			system(command.c_str());
		}
	}
  
}

inline void launch_game(const std::string &instance_name, const std::string &instance_type, const std::string &instance_version, bool untouched)
{
	//TODO: Get help for windows and macos. I can't test it.
	//And I don't even know how to launch the game on macos in the first place.
	if(!global::lock)
	{
		
			if(instance_type == "Custom"? std::filesystem::exists(instance_version) : std::filesystem::exists("download/" + instance_name))
			{
				std::string es_folder;
				std::string os = get_OS();
				std::string game_executable{""};
				if(os == "Linux")
				{
					es_folder = std::getenv("HOME") + std::string("/.local/share/endless-sky");
					game_executable = "endless-sky.AppImage";
				}
				else if(os == "Windows")
				{
					es_folder = std::getenv("APPDATA") + std::string("/endless-sky");
					game_executable = "EndlessSky.exe";
				}
				else if(os == "MacOS")
				{
					es_folder = std::getenv("HOME") + std::string("/Library/Application Support/endless-sky");
					game_executable = "EndlessSky";
				}
				bool plugins_exists{std::filesystem::exists(es_folder + "/plugins")};
				bool _plugins_exists{std::filesystem::exists(es_folder + "/_plugins")};
				std::string target_folder_plugins{untouched ? es_folder + "/plugins" : es_folder + "/_plugins"};

				if(untouched && plugins_exists)
				{
					//C++'s native filesystem library doesn't support moving non-empty directories, so we have to use this handy C function.

					rename((es_folder + "/plugins").c_str(), (es_folder + "/_plugins").c_str());
				}
				else if(!untouched && _plugins_exists)
				{
					if(plugins_exists)
					{
						std::filesystem::remove_all(es_folder + "/plugins");
					}
					rename((es_folder + "/_plugins").c_str(), (es_folder + "/plugins").c_str());
				}
				std::string command{""};
				std::string game_command{""};
				if(instance_type != "Custom")
				{
					command = "chmod +x \"download/" + instance_name + "/" + game_executable + "\"";
					game_command = "download/" + instance_name + "/" + game_executable;
				}
				else
				{
					command = "chmod +x \"" + instance_version + "\"";
					game_command = instance_version;
				}
				
				system(command.c_str());
				#ifdef __linux__
				char *const  args[] = {(char *)game_command.c_str()};
				pid_t pid = fork();
				switch(pid) 
				{
					case 0: 
						execvp(game_command.c_str(), args);
						break;
					case -1: 
						std::cout << "error\n";
				}
				#elif _WIN32
				if(instance_type != "Custom")
				{
					bool run_extraction_command{false};
					if(!std::filesystem::exists("download/" + instance_name + "/" + game_executable))
					{
						run_extraction_command = true;
						command = "7za x \"download/" + instance_name + "/EndlessSky-win64.zip\" -o\"download/" + instance_name + "\"";
					}
					if(run_extraction_command)
					{
						system(command.c_str());
					}
				}
				system(game_command.c_str());
				#endif
			}
			else 
			{
				Gtk::Dialog warn;
				
				Gtk::HeaderBar header;
				header.set_show_close_button();
				warn.set_titlebar(header);
				Gtk::Label warning;
				Gtk::Image image_warning;
				image_warning.set_from_icon_name("dialog-warning", Gtk::ICON_SIZE_DIALOG);
				warning.set_text("The selected game installation could not be found.\nPlease redownload the game.");
				
				warn.get_content_area()->pack_start(image_warning);
				warn.get_content_area()->pack_start(warning);

				warn.set_title("Error");
				warn.show_all();
				warn.add_button("OK", 1);
				switch(warn.run())
				{
					case 1:
						warn.close();
						break;
				}
			}
		
		/*else
		{
			std::string command;
			std::string game_command;
			
			command = "chmod +x \"" + instance_version + "\"";
			game_command = instance_version;
			
			system(command.c_str());
			#ifdef __linux__
			char *const  args[] = {(char *)game_command.c_str()};
			pid_t pid = fork();
			switch(pid) {
				case 0: 
					execvp(game_command.c_str(), args);
					break;
				case -1: 
					std::cout << "error\n";
			}
			#elif _WIN32
			game_command = "start \"" + instance_version + "\"";
			system(game_command.c_str());
			#endif
		}*/ 
	}
	else
	{
		Gtk::Dialog warndialog;
		Gtk::Image image_warning;
		image_warning.set_from_icon_name("dialog-warning", Gtk::ICON_SIZE_DIALOG);
		warndialog.set_title("Error");
		Gtk::Label warndialoglabel;
		warndialoglabel.set_text("There is an instance being downloaded.\nPlease wait until it finishes.");
		warndialog.get_content_area()->pack_start(image_warning);
		warndialog.get_content_area()->pack_start(warndialoglabel);
		
		warndialog.show_all();
		warndialog.add_button("OK", 1);
		switch(warndialog.run())
		{
			case 1:
				warndialog.close();
				break;
		}
	}
}
inline void download(const std::string &type, Gtk::ProgressBar * prog, Gtk::Window * win, const std::string &instance_name, const std::string &instance_version)
{
  /*if(!global::lock)
  {*/
    //global::lock = true;
    std::thread t(std::bind(aria2Thread, prog, type, instance_name, instance_version, win, false));
    t.detach();
  //}
}