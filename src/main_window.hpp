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
#include "glibmm/refptr.h"
#include "global_variables.hpp"
#include "gtkmm/button.h"
#include "gtkmm/dialog.h"
#include "gtkmm/headerbar.h"
#include "gtkmm/hvbox.h"
#include "gtkmm/image.h"
#include "gtkmm/label.h"
#include "gtkmm/notebook.h"
#include "gtkmm/progressbar.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/widget.h"
#include "instance.hpp"
#include "plugin_instance.hpp"

//Removes the instance from the list of instances using its name
inline void remove_instance(std::string name, std::vector<Instance> *instances, std::vector<Gtk::Button> *instance_buttons, Gtk::Window * window)
{
    for (int i{0}; i < instances->size(); i++)
    {
        if(instances->at(i).get_name() == name)
        {
            instances->at(i).get_rekt();
            instances->erase(instances->begin() + i);
            instance_buttons->erase(instance_buttons->begin() + i);
            window->show_all();
            return;
        }
    }
    
}
//Adds the '^' symbol, used as a separator for the spaces in the instance name in the file, since the ifstream is a pain in the @$$
inline std::string put_circunflexes(std::string str)
{
    std::string new_str;
    for (int i{0}; i < str.size(); i++)
    {
        if(str[i] == ' ')
        {
            new_str += '^';
        }
        else
        {
            new_str += str[i];
        }
    }
    return new_str;
}
//Removes the '^' symbol, used as a separator for the spaces in the instance name in the file, since the ifstream is a pain in the @$$
inline std::string put_spaces(std::string str)
{
    std::string new_str;
    for(int i{0}; i < str.size(); i++)
    {
        if(str[i] == '^')
        {
            new_str += ' ';
        }
        else
        {
            new_str += str[i];
        }
    }
    return new_str;
}
//Saves the instances to the disk
inline void save_instances(std::vector<Instance> * instances)
{
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
                Gtk::Dialog warn;
                warn.set_title("Warning");
                warn.get_action_area()->pack_start(*Gtk::manage(new Gtk::Label("An instance with this name already exists.")));
                warn.add_button("OK", 1);
                warn.show_all();
                switch(warn.run())
                {
                    case 1:
                        warn.close();
                        break;
                }
                return;
            }
        }
        instances.push_back(Instance(name, type, version, &progress, win, autoupdate, untouched));
        instances.back().set_autoupdate(autoupdate);
        instances.back().set_untouched(untouched);
        auto * tmp = instances[instances.size() - 1].get_labels_box();

        instance_buttons.push_back(Gtk::Button());
        tmp->pack_start(instance_buttons[instance_buttons.size()-1]);
        instance_buttons[instance_buttons.size() -1].set_image_from_icon_name("user-trash-symbolic");
        instance_buttons[instance_buttons.size()-1].signal_clicked().connect(sigc::bind(sigc::ptr_fun(&remove_instance), instances[instances.size()-1].get_name(), &instances, &instance_buttons, this));
        this->signal_delete_event().connect(sigc::bind(sigc::ptr_fun(&on_deelete_event),  &instances));

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
    Gtk::Button m_new_instance_button, m_open_data_folder_button, m_uninstall_all_button;
    Gtk::ProgressBar progress;
    Gtk::VBox m_vbox;
    Gtk::HeaderBar titlebar;    
    Gtk::Notebook m_notebook;
    Gtk::VBox m_plugins_vbox;
    Gtk::ScrolledWindow m_plugins_scrolled_window;
    bool generated_plugins{false};
    
};
inline void on_switch_page(Gtk::Widget * page, guint number, Gtk::VBox * m_plugins_vbox, bool &generated_plugins)
{
    if(number == 1 && !generated_plugins)
    {
        for(auto & p : global::plugins)
        {
            //PluginInstance instance{p};
            m_plugins_vbox->pack_start(*Gtk::manage(new PluginInstance(p)));
        }
        //m_plugins_vbox->pack_start(*Gtk::manage(new Gtk::Label("This is a placeholder for the plugins page.")));
        m_plugins_vbox->show_all();
        generated_plugins = true;
    }
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
            window->add_instance(dialog.get_naem(), dialog.get_typee(), dialog.get_version(), window, dialog.auto_update(), dialog.vanilla());
            break;
    }
}
//Removes all instances and the .local/share/endless-sky folder
inline void uninstall_all(Gtk::ProgressBar * progress, MyWindow * mywindow)
{
    Gtk::Dialog warn;
    Gtk::HeaderBar titlebar;

    titlebar.set_show_close_button();
    warn.set_titlebar(titlebar);
    titlebar.set_title("Warning");
    Gtk::Image *image = Gtk::manage(new Gtk::Image());
    image->set_from_icon_name("dialog-warning", Gtk::ICON_SIZE_DIALOG);
    Gtk::VBox *warn_vbox = Gtk::manage(new Gtk::VBox());
    warn_vbox->pack_start(*image);
    warn.get_action_area()->pack_start(*warn_vbox);
    warn_vbox->pack_start(*Gtk::manage(new Gtk::Label("Are you sure you want to uninstall all instances?\nThis will delete all instances and the .local/share/endless-sky folder.\nThis action cannot be undone.")));
    warn_vbox->get_children()[1]->set_margin_top(10);
    warn_vbox->get_children()[1]->set_margin_bottom(10);
    warn_vbox->get_children()[1]->set_margin_left(10);
    warn_vbox->get_children()[1]->set_margin_right(10);
    warn_vbox->get_children()[1]->set_halign(Gtk::ALIGN_CENTER);
    
    Gtk::Button * cancel = warn.add_button("Cancel", 1);
    Gtk::Button * uninstall = warn.add_button("Uninstall", 2);
    warn.get_action_area()->remove(*cancel);
    warn.get_action_area()->remove(*uninstall);

    warn_vbox->pack_start(*cancel);
    warn_vbox->pack_start(*uninstall);

    warn.show_all();
    switch(warn.run())
    {
        case 1:
            warn.close();
            break;
        case 2:
            warn.close();
            progress->set_fraction(0);
            progress->set_text("Uninstalling...");
            progress->show();
            while(Gtk::Main::events_pending())
            {
                Gtk::Main::iteration();
            }
            std::vector<Instance> instances = read_instances(progress, mywindow);
            for (auto & p : instances)
            {
                remove_instance(p.get_name(), &instances, mywindow->get_instance_buttons(), mywindow);
            }
            
            std::string command = "rm -rf ~/.local/share/endless-sky";
            if(get_OS() == "Windows")
            {
                command = "rmdir /s /q %APPDATA%\\.local\\share\\endless-sky";
            }
            else if(get_OS() == "MacOS")
            {
                command = "rm -rf ~/Library/Application\\ Support/endless-sky";
            }
            system(command.c_str());
            progress->set_fraction(0);
            progress->set_text("Done!");
            break;
    }
    mywindow->show_all();
}
//Opens the data folder in the file manager
inline void open_data_folder()
{
    if(get_OS() == "Linux")
    {
        std::string command = "xdg-open " + std::string(getenv("HOME")) + "/.local/share/endless-sky/";
        system(command.c_str());
    }
    else if(get_OS() == "Windows")
    {
        std::string command = "explorer " + std::string(getenv("APPDATA")) + "\\endless-sky\\";
        system(command.c_str());
    }
    else if(get_OS() == "MacOS")
    {
        std::string command = "open " + std::string(getenv("HOME")) + "/Library/Application Support/endless-sky/";
        system(command.c_str());
    }
}
//The MyWindow constructor. Puts the widgets in place and connects the signals
inline MyWindow::MyWindow()
{
    add(m_notebook);
    m_notebook.append_page(m_vbox, "Instances");
    
    titlebar.pack_start(m_new_instance_button);
    titlebar.pack_start(m_open_data_folder_button);
    titlebar.pack_start(m_uninstall_all_button);
    m_open_data_folder_button.set_image_from_icon_name("folder-symbolic");
    m_new_instance_button.set_image_from_icon_name("document-new");
    m_uninstall_all_button.set_image_from_icon_name("user-trash-symbolic");
    m_vbox.set_border_width(10);
    m_vbox.set_spacing(10);
    m_vbox.set_valign(Gtk::ALIGN_START);
    m_vbox.pack_start(progress);
    m_open_data_folder_button.signal_clicked().connect(sigc::ptr_fun(&open_data_folder));
    m_new_instance_button.signal_clicked().connect(sigc::bind(sigc::ptr_fun(new_dialog), this));
    m_uninstall_all_button.signal_clicked().connect(sigc::bind(sigc::ptr_fun(&uninstall_all), &progress, this));

    m_notebook.append_page(m_plugins_scrolled_window, "Plugins");
    m_plugins_scrolled_window.add(m_plugins_vbox);
    
    m_plugins_vbox.set_border_width(10);

    m_notebook.signal_switch_page().connect(sigc::bind(sigc::ptr_fun(on_switch_page),&m_plugins_vbox, generated_plugins));

    set_titlebar(titlebar);
    titlebar.set_show_close_button();
    set_title("ESThrower");
    titlebar.set_subtitle("Endless Sky Launcher");
    set_default_size(800, 600);
}

