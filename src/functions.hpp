#pragma once
#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include "global_variables.hpp"
#include "aria.hpp"
#include <thread>
#include <unistd.h>

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
  #ifdef __linux__
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
  #elif _WIN32
    std::string command = "explorer \"download/" + instance_name + "\"";
    system(command.c_str());
  #elif __APPLE__ || __MACH__
    std::string command = "open \"download/" + instance_name + "\"";
    system(command.c_str());
  #endif
  
}

inline void launch_game(const std::string &instance_name, const std::string &instance_type, const std::string &instance_version)
{
  #ifdef __linux__
  if(instance_type != "Custom")
  {
    if(std::filesystem::exists("download/" + instance_name))
    {
        std::string command;
        std::string game_command;
		if(get_OS() == "Linux")
		{
			if(instance_type == "Continuous")
			{
				command = "chmod +x \"download/" + instance_name + "/endless-sky-x86_64-continuous.AppImage\"";
				game_command = "download/" + instance_name + "/endless-sky-x86_64-continuous.AppImage";
			}
			else if(instance_type == "Stable")
			{
				command = "chmod +x \"download/" + instance_name + "/endless-sky-amd64-" + instance_version + ".AppImage\"";
				game_command = "download/" + instance_name + "/endless-sky-amd64-" + instance_version + ".AppImage";
			}
		}
		else if(get_OS() == "Windows")
		{
			if(instance_type == "Continuous")
			{
				command = "7z x \"download/" + instance_name + "/EndlessSky-win64-continuous.zip\" -o\"download/" + instance_name + "\"";
				game_command = "download/" + instance_name + "/EndlessSky-win64-continuous/EndlessSky.exe";
			}
			else if(instance_type == "Stable")
			{
				command = "7z x \"download/" + instance_name + "/endless-sky-win64-" + instance_version_minus_v(instance_version) + ".zip\" -o\"download/" + instance_name + "\"";
				game_command = "download/" + instance_name + "/endless-sky-win64-" + instance_version_minus_v(instance_version) + "/EndlessSky.exe";
			}
		}
        
        system(command.c_str());
        
        char *const  args[] = {(char *)game_command.c_str()};
        pid_t pid = fork();
        switch(pid) {
            case 0: 
                execvp(game_command.c_str(), args);
                break;
            case -1: 
                std::cout << "error\n";
        }
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
  }
  else
  {
		std::string command;
        std::string game_command;
        
		command = "chmod +x \"" + instance_version + "\"";
		game_command = instance_version;
        
        system(command.c_str());
        
        char *const  args[] = {(char *)game_command.c_str()};
        pid_t pid = fork();
        switch(pid) {
            case 0: 
                execvp(game_command.c_str(), args);
                break;
            case -1: 
                std::cout << "error\n";
        }
  } 
    
  
  #elif _WIN32
    if(std::filesystem::exists("download/" + instance_name + "/"))
    {
        std::string command;
        if(instance_type == "Continuous")
        {
            command = "start /c \"download/" + instance_name + "/EndlessSky-win64-continuous/EndlessSky.exe\"";
        }
        else if(instance_type == "Stable")
        {
            command = "start /c \"download/" + instance_name + "/EndlessSky-win64-" + instance_version + "/EndlessSky.exe\"";
        }
        system(command.c_str());
      
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
     
  #endif
  
}
inline void download(const std::string &type, Gtk::ProgressBar * prog, const std::string &instance_name, const std::string &instance_version)
{
  if(!global::lock)
  {
    global::lock = true;
    std::thread t(std::bind(aria2Thread, prog, type, instance_name, instance_version));
    t.detach();
  }
}