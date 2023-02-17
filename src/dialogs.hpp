#pragma once
#include "gtkmm/checkbutton.h"
#include <gtkmm.h>
#include <iostream>
//When creating an instance, this dialog will appear.
//It will ask the user to:
    //1. Introduce the type of instance (For now: Stable, Continuous and Custom)
    //2. Introduce the name of the instance
    //3. If the user chooses "Custom", it will ask for the executable of the instance
    //4. If the user chooses "Stable", it will ask for the version of the instance
    //5. If the user chooses "Continuous", won't ask for anything
//Then it will create a new instance with the given information
//The instance is another class, found in the "instance.hpp" file
class NewInstanceDialog : public Gtk::Dialog
{
  public:
    NewInstanceDialog();
    
    //Getters
    bool auto_update();
    bool vanilla();
    int get_selected();
    Gtk::HBox * get_buttons_box();
    //Returns the name of the instance
    //Note the naem(), since get_name() is already used by Gtk::Dialog
    std::string get_naem();
    //Returns the version of the instance. Unless it's continuous, which will return "0"
    std::string get_version();
    //This returns the type of the instance. Please note that it has typee() instead of type(), since the get_type()
    //function is already defined in the Gtk::Dialog class (And returns a bunch of weird numbers)
    std::string get_typee();
    
  private:
    Gtk::Button cancel_button{"Cancel"};
    Gtk::HeaderBar header;
    Gtk::Entry version_entry;
    Gtk::ComboBoxText instance_type_combo;
    Gtk::HBox buttons_box;
    Gtk::CheckButton autoupdate, untouched;
    //Exclusive for Custom instances
    Gtk::Button browse_button;
    Gtk::HBox entry_and_button;
    Gtk::Entry name_entry;
    //Widget callbacks
    //Callback for the cancel button
    void on_cancel_button_clicked();
    //Callback for the browse button (In case of custom instances)
    void on_browse_button_clicked();
    void on_combo_changed();
    //Other functions
    void remove_autoupdate_untouched();
    void add_autoupdate_untouched(bool autoupdate_possible);
      
};
