#include "main_window.hpp"

//The MyWindow constructor. Puts the widgets in place and connects the signals
MyWindow::MyWindow()
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

    m_notebook.signal_switch_page().connect(sigc::mem_fun(*this, &MyWindow::on_switch_page));
    this->signal_delete_event().connect(sigc::mem_fun(*this, &MyWindow::on_deleete_event));
    set_titlebar(titlebar);
    titlebar.set_show_close_button();
    set_title("ESThrower");
    titlebar.set_subtitle("Endless Sky Launcher");
    set_default_size(800, 600);
}

void MyWindow::add_instance(std::string name, std::string type, std::string version, Gtk::Window *win, bool autoupdate, bool untouched)
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
    instance_buttons[instance_buttons.size() - 1].signal_clicked().connect(sigc::bind(sigc::mem_fun(*this, &MyWindow::remove_instance), instances[instances.size()-1].get_name()));
    instance_buttons[instance_buttons.size() - 1].set_tooltip_text("Delete instance");
    instances[instances.size() - 1].show_all();
    m_vbox.pack_start(instances[instances.size() - 1]);
}

std::vector<Instance> * MyWindow::get_instances()
{
    return &instances;
}

Gtk::ProgressBar * MyWindow::get_progress()
{
    return &progress;
}

std::vector<Gtk::Button> * MyWindow::get_instance_buttons()
{
    return &instance_buttons;
}

//Generates the plugins page, so it doesn't have to be generated at the first run, slowing down the initial load
void MyWindow::on_switch_page(Gtk::Widget * page, guint number)
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
            m_plugins_vbox.pack_start(*Gtk::manage(new PluginInstance(p, is_installed)));
        }
        m_plugins_vbox.show_all();
        generated_plugins = true;
    }
}

//Fired when the window is closed. Saves the instances and closes the window.
//Note the use of the 'deelete' word, since the on_delete_event is an already defined function in Gtk::Window
bool MyWindow::on_deleete_event(GdkEventAny* any_event)
{
    //Delete the temporary file /tmp/esthrower.lock
    std::remove("/tmp/esthrower.lock");

    save_instances();
    return false;
}

//Saves the instances to the disk as JSON objects
void MyWindow::save_instances()
{
    if(instances.size() == 0)
    {
        std::ofstream file("instances.json");
        file << "[]";
        file.close();
        return;
    }
    nlohmann::json j;
    for (auto & p : instances)
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

//Removes the instance from the list of instances using its name
inline void MyWindow::remove_instance(std::string name)
{
    
    for (unsigned long i{0}; i < instances.size(); i++)
    {
        if(instances.at(i).get_name() == name)
        {
            std::cout << "[INFO] Deleting instance " << name << std::endl;
            DeletingInstanceDialog dialog("Are you sure you want to delete " + name + "?\nThis will remove the instance.");
            dialog.run();
            if(dialog.cancelled())
            {
                break;
            }
            instances.erase(instances.begin() + i);
            instance_buttons.erase(instance_buttons.begin() + i);

            DeletingInstanceDialog dialog2("Do you want to delete the instance folder?\nThis will remove the instance folder and all its contents.");
            dialog2.run();
            if(dialog2.cancelled())
            {
                break;
            }
            if(std::filesystem::exists("instances/" + name))
            {
                instances.at(i).get_rekt();
            }
            break;
        }
    }
    show_all();
}