#include <cstddef>
#include <gtkmm.h>

#include <iostream>
#include <thread>
#include <filesystem>
#include <unistd.h>

#include "gtkmm/progressbar.h"
#include "main_window.hpp"

int main(int argc, char* argv[])
{
  #ifdef __linux
  aria2::libraryInit();
  #endif
  auto app = Gtk::Application::create("org.gtkmm.examples.base");
  MyWindow win;
  
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
  #ifdef __linux
  aria2::libraryDeinit();
  #endif
  return 0;
}
