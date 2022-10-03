#pragma once
#include <gtkmm.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "dialogs.hpp"
#include "functions.hpp"
#include "glibmm/refptr.h"
#include "gtkmm/button.h"
#include "gtkmm/dialog.h"
#include "gtkmm/headerbar.h"
#include "gtkmm/hvbox.h"
#include "gtkmm/image.h"
#include "gtkmm/label.h"
#include "gtkmm/progressbar.h"
#include "instance.hpp"
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
    std::ofstream file;
    file.open("download/instances.txt");
    for (auto & p : *instances)
    {
        file << put_circunflexes(p.get_name()) << " " << p.get_typee() << " " << put_circunflexes(p.get_version()) << std::endl;
    }
    file.close();
}
//Loads the instances from the disk
inline std::vector<Instance> read_instances(Gtk::ProgressBar * global_prog)
{
    std::vector<Instance> ret;
    std::ifstream file;
    file.open("download/instances.txt");
    std::string line;
    while(std::getline(file, line))
    {
        bool foundit{false};
        std::string name, type, version;
        std::stringstream line_ss{line};
        line_ss >> name >> type >> version;
        name = put_spaces(name);
        version = put_spaces(version); //The version is used for the custom builds as a filename instead of a version number. This is rather hackish, but... do we need to show a version number for an instance without it?
        
        ret.emplace_back(name, type, version, global_prog);
    }
    file.close();
    return ret;
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
    void add_instance(std::string name, std::string type, std::string version)
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
        instances.push_back(Instance(name, type, version, &progress));
        Gtk::HBox *tmp = instances[instances.size()-1].get_labels_box();

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
};
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
            window->add_instance(dialog.get_naem(), dialog.get_typee(), dialog.get_version());
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
            std::vector<Instance> instances = read_instances(progress);
            for (auto & p : instances)
            {
                remove_instance(p.get_name(), &instances, mywindow->get_instance_buttons(), mywindow);
            }
            std::string command = "rm -rf ~/.local/share/endless-sky";
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
    add(m_vbox);
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

    set_titlebar(titlebar);
    titlebar.set_show_close_button();
    set_title("ESThrower");
    titlebar.set_subtitle("Endless Sky Launcher");
    set_default_size(800, 600);
}

