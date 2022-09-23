#pragma once
#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include "global_variables.hpp"
#include "aria.hpp"
#include <thread>
#include <unistd.h>

inline void open_folder(std::string instance_name)
{
  #ifdef __linux__
    std::string command = "xdg-open \"download/" + instance_name + "\"";
    system(command.c_str());
  #elif _WIN32
    std::string command = "explorer \"download/" + instance_name + "\"";
    system(command.c_str());
  #elif __APPLE__ || __MACH__
    std::string command = "open \"download/" + instance_name + "\"";
    system(command.c_str());
  #endif
  
}

inline void launch_game(const std::string &instance_name, const std::string &instance_type)
{
  #ifdef __linux__
    if(std::filesystem::exists("download/" + instance_name))
    {
        std::string command;
        if(instance_type == "Continuous")
        {
            command = "chmod +x \"download/" + instance_name + "/endless-sky-x86_64-continuous.AppImage\"";
        }
        std::string game_command = "download/" + instance_name + "/endless-sky-x86_64-continuous.AppImage";
        
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
            command = "start /c \"download/" + instance_name + "/EndlessSky-win64-" + version + "/EndlessSky.exe\"";
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
inline void download(const std::string &type, Gtk::ProgressBar * prog, const std::string &instance_name)
{
  if(!global::lock)
  {
    global::lock = true;
    std::thread t(std::bind(aria2Thread, prog, type, instance_name));
    t.detach();
    //aria2Thread(prog, type, instance_name);
  }
}