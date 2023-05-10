#include "instance_widget.hpp"
#include "downloader.hpp"
#include "functions.hpp"
#include "global_variables.hpp"
#include "gtkmm/box.h"
#include "gtkmm/enums.h"
#include "sigc++/functors/ptr_fun.h"

//As much as I'd like to put this in the Intance class, I can't. For some reason, it doesn't work.
//No idea why. I've tried with:
// - sigc::mem_fun(*this, &Instance::download_signal_handler)
// - marking it as static
// - marking it as static and using sigc::ptr_fun (Errors out)
//What really makes this baffling is that the mem_fun thingy works on other classes.
void download_signal_handler(Gtk::ProgressBar * global_prog, std::string type, std::string name, std::string version, Gtk::Window * window)
{
    //std::cout << "[INFO] Pressed download button for: " << get_name() << std::endl;
    Instance::download_priv(global_prog, type, name, version, window);
}

Instance::Instance(std::string name, std::string _type, std::string _version, Gtk::ProgressBar * global_prog, Gtk::Window * win, bool autoupdate, bool untouched)
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
    labels_box.pack_end(button_box);
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

    button_box.pack_end(folder);
    folder.set_image_from_icon_name("folder-symbolic");
    folder.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(Functions::open_folder), get_name(), get_typee(), get_version()));
    folder.set_tooltip_text("Open instance folder");
    if(type != "Custom")
    {
        labels_box.pack_start(update);
    }
    update.set_image_from_icon_name("go-down");
    
    update.set_tooltip_text("Update instance");
    button_box.pack_end(launch);
    launch.set_image_from_icon_name("media-playback-start");
    launch.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(Functions::launch_game), get_name(), type, version, untouched));
    launch.set_tooltip_text("Launch the game");
    if(!get_untouched())
    {
        button_box.pack_end(run_without_plugins);
    }
    button_box.set_halign(Gtk::ALIGN_END);
    button_box.set_spacing(10);
    button_box.set_hexpand(false);
    button_box.set_valign(Gtk::ALIGN_CENTER);
    run_without_plugins.set_image_from_icon_name("video-display-symbolic");
    run_without_plugins.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(Functions::launch_game), get_name(), type, version, true));
    //Put a small label when hovering over the button
    run_without_plugins.set_tooltip_text("Launch the game without plugins");
    update.signal_clicked().connect(sigc::bind(sigc::ptr_fun(download_signal_handler), global_prog, type, name, version, window));

    show_all();
}



void Instance::set_untouched(bool untouched)
{
    this->untouched = untouched;
}

bool Instance::get_untouched()
{
    return untouched;
}

void Instance::set_autoupdate(bool autoupdate)
{
    this->autoupdate = autoupdate;
}

bool Instance::get_autoupdate()
{
    return autoupdate;
}

std::string Instance::get_version()
{
    return version;
}

std::string Instance::get_name()
{
    return name_label.get_text();
}

std::string Instance::get_typee()
{
    return type;
}

Gtk::HBox * Instance::get_button_box()
{
    return &button_box;
}

void Instance::get_rekt()
{
    if(type != "Custom")
    {
        std::filesystem::remove_all(global::config_dir + "download/" + get_name());
    }
    else 
    {
        //Get the path to the folder from the filename contained in "version"
        std::string path = version.substr(0, version.find_last_of("/"));
        std::filesystem::remove_all(path);
    }
}

void Instance::download()
{
    download_priv(global_prog, get_typee(), get_name(), get_version(), window);
}

void Instance::download_priv(Gtk::ProgressBar * global_prog, std::string type, std::string name, std::string version, Gtk::Window * window)
{    
    Downloader d;
    //It's funny that I need an object to call a non-static function
    std::thread t(&Downloader::download_instance, &d, global_prog, type, name, version, window);
    t.detach();
}

