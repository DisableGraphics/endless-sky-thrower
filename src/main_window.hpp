#pragma once
#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "dialogs.hpp"
#include "global_variables.hpp"
#include "gtkmm/hvbox.h"
#include "instance_widget.hpp"
#include "plugin_instance.hpp"
#include "changelog_widget.hpp"
#include "icon/esthrower.xpm"
#include "secondary_dialogs.hpp"
#include "nlohmann/json.hpp"

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
//The main window. Inherits from Gtk::Window
class MyWindow : public Gtk::Window
{
  public:
    MyWindow();
    //Adds an intance to the list of instances and shows it in the window
    void add_instance(std::string name, std::string type, std::string version, Gtk::Window * win, bool autoupdate, bool untouched);
    //Returns the list of instances
    std::vector<Instance> * get_instances();
    //Returns the progress bar
    Gtk::ProgressBar * get_progress();
    std::vector<Gtk::Button> * get_instance_buttons();
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
    ChangelogWidget changelog;
    bool generated_plugins{false};

    void save_instances();
    void remove_instance(std::string name);

    bool on_deleete_event(GdkEventAny* any_event);
    void on_switch_page(Gtk::Widget *page, guint number);
};

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
    command = "cd download/" + pr_number + " && cmake . && cmake --build .";
    system(command.c_str());
    if(Functions::get_OS() != "Windows")
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
    std::string os{Functions::get_OS()};
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