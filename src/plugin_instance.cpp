#include "plugin_instance.hpp"
#include "global_variables.hpp"

void PluginInstance::set_installed(bool installed)
{
    if(installed)
    {
        install_button.set_label("Update");
        pack_start(uninstall_button);
    }
    else 
    {
        install_button.set_label("Install");
        remove(uninstall_button);
    }
    
    show_all();
}

PluginInstance::PluginInstance(Plugin_ID id, bool is_installed)
{
    spinner.stop();
    plugin_id = id;
    set_spacing(10);
    set_border_width(10);

    set_halign(Gtk::ALIGN_FILL);
    set_valign(Gtk::ALIGN_START);
    set_hexpand(true);
    set_vexpand(true);
    

    name_label.set_markup("<b>Name: </b>" + plugin_id.name);

    version_label.set_markup("<b>Version: </b>" + plugin_id.version);

    author_label.set_markup("<b>Author: </b>" + plugin_id.author);

    description_label.set_markup("<b>Description: </b>" + plugin_id.description);
    description_label.set_line_wrap();
    uninstall_button.set_label("Uninstall");
    if(is_installed)
    {
        install_button.set_label("Update");
    }
    else
    {
        install_button.set_label("Install");
    }

    pack_start(spinner);
    pack_start(separator2);
    pack_start(name_label);
    pack_start(version_label);

    pack_start(author_label);
    pack_start(description_label);
    pack_start(install_button);
    uninstall_button.signal_clicked().connect([&](){std::thread t(std::bind(uninstall_plugin, this)); t.detach();});
    install_button.signal_clicked().connect([&](){std::thread t(std::bind(download_plugin, this)); t.detach();});
    if(is_installed)
    {
        pack_start(uninstall_button);
    }

    
    separator.set_margin_top(10);
    //The last one is the separators
    pack_end(separator);
    show_all();
}

Plugin_ID PluginInstance::get_plugin_id()
{
    return plugin_id;
}

Gtk::Spinner * PluginInstance::get_spinner()
{
    return &spinner;
}