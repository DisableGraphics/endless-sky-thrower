#include <filesystem>
#include <gtkmm.h>
#include <iostream>
#include "functions.hpp"
#include "gtkmm/hvbox.h"
//Instance class, used to store the instance data. Inherits from Gtk::HBox
class Instance : public Gtk::VBox
{
  public:
    //Constructor. Sets the name, the type, the version and a pointer to the progress bar
    Instance(std::string name, std::string _type, std::string _version, Gtk::ProgressBar * global_prog)
    {
        type = _type;
        set_spacing(10);
        name_label.set_text(name);
        bool has_v{false};
        for (char & aux : _version)
        {
            if (aux == 'v')
            {
                has_v = true;
            }
        }
        if(type != "Custom")
        {
            if(has_v)
            {
                version = _version;
            }
            else
            {
                version = "v" + _version;
            }
        }
        else 
        {
            version = _version;
        }
        pack_start(separator1);
        pack_end(separator2);
        version_label.set_label(_version);

        pack_start(labels_box);
        labels_box.pack_start(name_label);
        labels_box.set_halign(Gtk::ALIGN_START);
        labels_box.set_spacing(10);
        if(type == "Continuous")
        {
            version_label.set_label("Continuous");
            labels_box.pack_start(version_label);
        }
        else if(type == "Custom")
        {
            version_label.set_label("Custom");
            labels_box.pack_start(version_label);
        }
        else
        {
            labels_box.pack_start(version_label);
        }

        labels_box.pack_start(folder);
        folder.set_image_from_icon_name("folder-symbolic");
        folder.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(&open_folder), get_name(), get_typee(), get_version()));
        
        if(type != "Custom")
        {
            labels_box.pack_start(update);
        }
        update.set_image_from_icon_name("go-down");
        update.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(&download), type, global_prog, get_name(), get_version()));

        labels_box.pack_start(launch);
        launch.set_image_from_icon_name("media-playback-start");
        launch.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(&launch_game), get_name(), type, version));
        
        show_all();
    }
    //Returns the version
    std::string get_version()
    {
        return version;
    }
    //Returns the name
    std::string get_name()
    {
        return name_label.get_text();
    }
    //Returns the type
    std::string get_typee()
    {
        return type;
    }
    //Returns the main box
    Gtk::HBox * get_labels_box()
    {
        return &labels_box;
    }
    //Deletes everything related to this instance. Unless it's a custom instance, then you'll have to manually delete the folder.
    void get_rekt()
    {
        if(type != "Custom")
        {
            std::filesystem::remove_all("download/" + get_name());
        }
    }
    
  private:
    std::string type;
    Gtk::Separator separator1, separator2;
    std::string version;
    Gtk::Label name_label;
    Gtk::Label version_label;
    Gtk::Button launch;
    Gtk::Button delete_button;
    Gtk::Button folder;
    Gtk::Button update;
    Gtk::HBox labels_box;
};