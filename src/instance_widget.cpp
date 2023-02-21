#include "instance_widget.hpp"
#include "downloader.hpp"
#include "functions.hpp"

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
    folder.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(Functions::open_folder), get_name(), get_typee(), get_version()));
    folder.set_tooltip_text("Open instance folder");
    if(type != "Custom")
    {
        labels_box.pack_start(update);
    }
    update.set_image_from_icon_name("go-down");
    
    update.set_tooltip_text("Update instance");
    labels_box.pack_start(launch);
    launch.set_image_from_icon_name("media-playback-start");
    launch.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(Functions::launch_game), get_name(), type, version, untouched));
    launch.set_tooltip_text("Launch the game");
    if(!get_untouched())
    {
        labels_box.pack_start(run_without_plugins);
    }
    run_without_plugins.set_image_from_icon_name("video-display-symbolic");
    run_without_plugins.signal_clicked().connect(sigc::bind<std::string>(sigc::ptr_fun(Functions::launch_game), get_name(), type, version, true));
    //Put a small label when hovering over the button
    run_without_plugins.set_tooltip_text("Launch the game without plugins");
    update.signal_clicked().connect(sigc::mem_fun(*this, &Instance::download));

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

Gtk::HBox * Instance::get_labels_box()
{
    return &labels_box;
}

void Instance::get_rekt()
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

void Instance::download()
{
    std::cout << "\nDownloading " << get_name() << " " << get_version() << std::endl;
    
    //This crashes with "Fatal error: glibc detected an invalid stdio handle"
    //May be related to the fact that download_instance is a static function
    Downloader d;
    std::thread t(&Downloader::download_instance, &d, global_prog, get_typee(), get_name(), get_version(), window);
    t.detach();
}

