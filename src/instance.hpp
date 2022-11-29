#include <filesystem>
#include <gtkmm.h>
#include <iostream>
#include "aria.hpp"
#include "functions.hpp"
#include "gtkmm/hvbox.h"
#include "gtkmm/progressbar.h"
#include "gtkmm/window.h"
#include "sigc++/adaptors/bind.h"
#include "sigc++/functors/ptr_fun.h"
//Instance class, used to store the instance data. Inherits from Gtk::HBox
class Instance : public Gtk::VBox
{
  public:
    //Constructor. Sets the name, the type, the version and a pointer to the progress bar
    Instance(std::string name, std::string _type, std::string _version, Gtk::ProgressBar * global_prog, Gtk::Window * win, bool autoupdate, bool untouched);
    void set_untouched(bool untouched)
    {
        this->untouched = untouched;
    }
    bool get_untouched()
    {
        return untouched;
    }
    void set_autoupdate(bool autoupdate)
    {
        this->autoupdate = autoupdate;
    }
    bool get_autoupdate()
    {
        return autoupdate;
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
    //Deletes everything related to this instance
    void get_rekt()
    {
        if(type != "Custom")
        {
            std::filesystem::remove_all("download/" + get_name());
        }
        else 
        {
            //Get the path to the folder from the filename contained in "version"
            std::string path = version.substr(0, version.find_last_of("/"));
            std::filesystem::remove_all(path);
        }
    }
    void download()
    {
        std::cout << "Downloading " << get_name() << " " << get_version() << std::endl;
        std::thread t(std::bind(aria2Thread, global_prog, get_typee(), get_name(), get_version(), window, false));
        t.detach();
    }
    
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

inline Instance::Instance(std::string name, std::string _type, std::string _version, Gtk::ProgressBar * global_prog, Gtk::Window * win, bool autoupdate, bool untouched)
{
    this->global_prog = global_prog;
    window = win;
    type = _type;
    set_spacing(10);
    name_label.set_text(name);
    bool has_v{false};
    this->autoupdate = autoupdate;
    this->untouched = untouched;
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
    
    untouched_label.set_label("Vanilla instance");
    untouched_label.set_valign(Gtk::ALIGN_CENTER);
    untouched_label.set_halign(Gtk::ALIGN_END);
    if(untouched)
    {
        labels_box.pack_end(untouched_label);
    }

    labels_box.pack_start(folder);
    folder.set_image_from_icon_name("folder-symbolic");
    folder.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(&open_folder), get_name(), get_typee(), get_version()));
    folder.set_tooltip_text("Open instance folder");
    if(type != "Custom")
    {
        labels_box.pack_start(update);
    }
    update.set_image_from_icon_name("go-down");
    update.signal_clicked().connect(sigc::bind(sigc::ptr_fun(&update_instance), global_prog, window, get_name(), get_typee(), get_version(), autoupdate, untouched));
    update.set_tooltip_text("Update instance");
    labels_box.pack_start(launch);
    launch.set_image_from_icon_name("media-playback-start");
    launch.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(&launch_game), get_name(), type, version, untouched));
    launch.set_tooltip_text("Launch the game");
    if(!get_untouched())
    {
        labels_box.pack_start(run_without_plugins);
    }
    run_without_plugins.set_image_from_icon_name("video-display-symbolic");
    run_without_plugins.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(&launch_game), get_name(), type, version, true));
    //Put a small label when hovering over the button
    run_without_plugins.set_tooltip_text("Launch the game without plugins");

    show_all();
}