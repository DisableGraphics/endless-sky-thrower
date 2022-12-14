#pragma once
#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "aria.hpp"
#include "dialogs.hpp"
#include "functions.hpp"
#include "global_variables.hpp"
#include "instance.hpp"
#include "plugin_instance.hpp"
#include "functions.hpp"
#include "icon/esthrower.xpm"
#include "secondary_dialogs.hpp"

//Removes the instance from the list of instances using its name
inline void remove_instance(std::string name, std::vector<Instance> *instances, std::vector<Gtk::Button> *instance_buttons, Gtk::Window * window)
{
    for (int i{0}; i < instances->size(); i++)
    {
        if(instances->at(i).get_name() == name)
        {
            std::cout << "[INFO] Deleting instance " << name << std::endl;
            instances->at(i).get_rekt();
            instances->erase(instances->begin() + i);
            instance_buttons->erase(instance_buttons->begin() + i);
            return;
        }
    }
    window->show_all();
}
//Saves the instances to the disk as JSON objects
inline void save_instances(std::vector<Instance> * instances)
{
    if(instances->size() == 0)
    {
        std::ofstream file("instances.json");
        file << "[]";
        file.close();
        return;
    }
    nlohmann::json j;
    for (auto & p : *instances)
    {
        j += nlohmann::json::object({
            {"name", p.get_name()},
            {"version", p.get_version()},
            {"type", p.get_typee()},
            {"vanilla", p.get_untouched()},
            {"autoupdate", p.get_autoupdate()}
        });
    }
    std::ofstream file;
    file.open("download/instances.json");
    file << j.dump(4);
    file.close();
}
//Loads the instances from the disk
inline std::vector<Instance> read_instances(Gtk::ProgressBar * global_prog, Gtk::Window * win)
{
    std::vector<Instance> instances;
    std::ifstream file;
    file.open("download/instances.json");
    if(file.is_open())
    {
        nlohmann::json j;
        file >> j;
        for (auto & p : j)
        {
            std::string name = p["name"];
            std::string version = p["version"];
            std::string type = p["type"];
            bool autoupdate = p["autoupdate"];
            bool untouched = p["vanilla"];
            instances.push_back(Instance(name, type, version, global_prog, win, autoupdate, untouched));
        }
    }
    else
    {
        std::cout << "[WARN] instances.json file not found. Resuming normal operation." << std::endl;
    }
    file.close();
    return instances;
}
//Fired when the window is closed. Saves the instances and closes the window.
//Note the use of the 'deelete' word, since the on_delete_event is an already defined function in Gtk::Window
inline bool on_deelete_event(GdkEventAny* any_event, std::vector<Instance> *instances)
{
    save_instances(instances);
    return false;
}
//The main window. Inherits from Gtk::Window
class MyWindow : public Gtk::Window
{
  public:
    MyWindow();
    //Adds an intance to the list of instances and shows it in the window
    void add_instance(std::string name, std::string type, std::string version, Gtk::Window * win, bool autoupdate, bool untouched)
    {
        for (auto & p : instances)
        {
            if(p.get_name() == name)
            {
                InformationDialog d("Instance already exists", "The instance " + name + " already exists. Please choose a different name.", true);
                d.show_all();
                d.run();
                return;
            }
        }
        instances.push_back(Instance(name, type, version, &progress, win, autoupdate, untouched));
        instances.back().set_autoupdate(autoupdate);
        instances.back().set_untouched(untouched);
        auto * tmp = instances[instances.size() - 1].get_labels_box();

        instance_buttons.push_back(Gtk::Button());
        tmp->pack_start(instance_buttons[instance_buttons.size()-1]);
        instance_buttons[instance_buttons.size() - 1].set_image_from_icon_name("user-trash-symbolic");
        instance_buttons[instance_buttons.size() - 1].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&remove_instance), instances[instances.size()-1].get_name(), &instances, &instance_buttons, (Gtk::Window *)this));
        instance_buttons[instance_buttons.size() - 1].set_tooltip_text("Delete instance");
        instances[instances.size() - 1].show_all();
        m_vbox.pack_start(instances[instances.size() - 1]);
    }
    //Returns the list of instances
    std::vector<Instance> * get_instances()
    {
        return &instances;
    }
    //Returns the progress bar
    Gtk::ProgressBar * get_progress()
    {
        return &progress;
    }
    std::vector<Gtk::Button> * get_instance_buttons()
    {
        return &instance_buttons;
    }
    
  private:
    std::vector<Gtk::Button> instance_buttons;
    std::vector<Instance> instances;
    Gtk::Button m_new_instance_button, m_open_data_folder_button;
    Gtk::ProgressBar progress;
    Gtk::VBox m_vbox;
    Gtk::HeaderBar titlebar;    
    Gtk::Notebook m_notebook;
    Gtk::VBox m_plugins_vbox;
    Gtk::ScrolledWindow m_plugins_scrolled_window, m_instances_scrolled_window;
    bool generated_plugins{false};
    
};
//Generates the plugins page, so it doesn't have to be generated at the first run, slowing down the initial load
inline void on_switch_page(Gtk::Widget * page, guint number, Gtk::VBox * m_plugins_vbox, bool &generated_plugins)
{
    if(number == 1 && !generated_plugins)
    {
        
        for(auto & p : global::plugins)
        {
            bool is_installed = is_plugin_installed(p.name);
            if(is_installed)
            {
                std::cout << "[INFO] Plugin " << p.name << " is installed." << std::endl;
            }
            m_plugins_vbox->pack_start(*Gtk::manage(new PluginInstance(p, is_installed)));
        }
        m_plugins_vbox->show_all();
        generated_plugins = true;
    }
}
inline void download_pr(std::string pr_number, Gtk::ProgressBar * progress_bar, MyWindow * win, NewInstanceDialog * dialog)
{
    while(global::lock)
    {
        sleep(1);
    }
    global::lock = true;
    std::string command{"git clone https://github.com/endless-sky/endless-sky.git download/" + pr_number
    + " && cd download/" + pr_number+ " && git pr " + pr_number};
    
    system(command.c_str());
    if(win->is_active())
    {
        progress_bar->set_fraction(0.5);
    }
    //Now compile it
    //Now that the Discord ppl said tha we were gonna use cmake...
    command = "cd download/" + pr_number + " && scons";
    system(command.c_str());
    if(get_OS() != "Windows")
    {
        InformationDialog d("Download complete", "The PR has been downloaded and compiled. You can now launch it.", true);
        d.show_all();
        d.run();
    }
    global::lock = false;
    while(!win->is_active())
    {
        sleep(1);
    }
    progress_bar->set_fraction(0);
}

//Creates a new dialog for creating a new instance
//Taking into account the pointer fuckery done here, I'm amazed this worked the first time I tried it.
inline void new_dialog(MyWindow * window)
{
    NewInstanceDialog dialog;
    dialog.set_title("New Instance");
    dialog.add_button("OK", 1);
    dialog.show_all();
    switch(dialog.run())
    {
        case 1:
            if(dialog.get_selected() == 3)
            {
                //It is a custom PR. We need to download it, compile it and set the version to the path of the executable
                std::thread t(download_pr, dialog.get_version(), window->get_progress(), window, &dialog);
                t.detach();
                window->add_instance(dialog.get_naem(), dialog.get_typee(), "download/" + dialog.get_version() + "/endless-sky", window, dialog.auto_update(), dialog.vanilla());
                
                InformationDialog d("Download started", "The download has started. Do not close the launcher while the download and compilation lasts.", true);
                d.show_all();
                d.run();

                return;
            }
            window->add_instance(dialog.get_naem(), dialog.get_typee(), dialog.get_version(), window, dialog.auto_update(), dialog.vanilla());
            break;
    }
}
//Opens the data folder in the file manager
inline void open_data_folder()
{
    std::string os{get_OS()};
    std::string command;
    if(os == "Linux")
    {
        command = "xdg-open " + std::string(getenv("HOME")) + "/.local/share/endless-sky/";
    }
    else if(os == "Windows")
    {
        command = "explorer " + std::string(getenv("APPDATA")) + "\\endless-sky\\";
    }
    else if(os == "MacOS")
    {
        command = "open " + std::string(getenv("HOME")) + "/Library/Application Support/endless-sky/";
   }
    system(command.c_str());
}
//The MyWindow constructor. Puts the widgets in place and connects the signals
inline MyWindow::MyWindow()
{
    add(m_notebook);
    m_notebook.append_page(m_instances_scrolled_window, "Instances");
    m_instances_scrolled_window.add(m_vbox);

    //Set the icon
    auto p = Gdk::Pixbuf::create_from_xpm_data(esthrower);
    set_icon(p);
    
    titlebar.pack_start(m_new_instance_button);
    titlebar.pack_start(m_open_data_folder_button);
    m_open_data_folder_button.set_image_from_icon_name("folder-symbolic");
    m_open_data_folder_button.set_tooltip_text("Open the Endless Sky data folder");
    m_new_instance_button.set_image_from_icon_name("document-new");
    m_new_instance_button.set_tooltip_text("Create a new instance");
    m_vbox.set_border_width(10);
    m_vbox.set_spacing(10);
    m_vbox.set_valign(Gtk::ALIGN_START);
    m_vbox.pack_start(progress);
    m_open_data_folder_button.signal_clicked().connect(sigc::ptr_fun(&open_data_folder));
    m_new_instance_button.signal_clicked().connect(sigc::bind(sigc::ptr_fun(new_dialog), this));

    m_notebook.append_page(m_plugins_scrolled_window, "Plugins");
    m_plugins_scrolled_window.add(m_plugins_vbox);
    
    m_plugins_vbox.set_border_width(10);

    m_notebook.signal_switch_page().connect(sigc::bind(sigc::ptr_fun(on_switch_page),&m_plugins_vbox, generated_plugins));
    this->signal_delete_event().connect(sigc::bind(sigc::ptr_fun(&on_deelete_event),  &instances));
    set_titlebar(titlebar);
    titlebar.set_show_close_button();
    set_title("ESThrower");
    titlebar.set_subtitle("Endless Sky Launcher");
    set_default_size(800, 600);
}

