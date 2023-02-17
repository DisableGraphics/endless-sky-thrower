#pragma once
#include <filesystem>
#include <gtkmm.h>
#include <iostream>
#include "downloader.hpp"
#include "functions.hpp"
#include "sigc++/adaptors/bind.h"
#include "sigc++/functors/ptr_fun.h"
//Instance class, used to store the instance data. Inherits from Gtk::HBox
class Instance : public Gtk::VBox
{
  public:
    //Constructor. Sets the name, the type, the version and a pointer to the progress bar
    Instance(std::string name, std::string _type, std::string _version, Gtk::ProgressBar * global_prog, Gtk::Window * win, bool autoupdate, bool untouched);
    void set_untouched(bool untouched);
    
    bool get_untouched();
    
    void set_autoupdate(bool autoupdate);
    
    bool get_autoupdate();
    
    //Returns the version
    std::string get_version();
    
    //Returns the name
    std::string get_name();
    
    //Returns the type
    std::string get_typee();
    
    //Returns the main box
    Gtk::HBox * get_labels_box();
    
    //Deletes everything related to this instance
    void get_rekt();
    void download();

  private:
    Gtk::Window * window;
    Gtk::ProgressBar * global_prog;
    std::string type;
    bool autoupdate{false};
    bool untouched{false};
    Gtk::Separator separator1, separator2;
    std::string version;
    Gtk::Label name_label;
    Gtk::Label version_label;
    Gtk::Button launch;
    Gtk::Button delete_button;
    Gtk::Button folder;
    Gtk::Button update;
    Gtk::Button run_without_plugins;
    Gtk::Label untouched_label;
    Gtk::HBox labels_box;
};
//This would not fire even if the button was pressed
inline void update_instance(Gtk::ProgressBar * prog, Gtk::Window * win, std::string name, std::string type, std::string version, bool autoupdate, bool untouched)
{
    std::thread t(std::bind(aria2Thread, prog, type, name, version, win, autoupdate));
    t.detach();
}