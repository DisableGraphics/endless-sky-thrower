#include "plugin_instance.hpp"
#include "functions.hpp"
#include "downloader.hpp"
#include "global_variables.hpp"
#include "sigc++/functors/mem_fun.h"
#include <thread>

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

    if(!Functions::str_replace(plugin_id.author, "&", "&amp;"))
    {
        std::cout << "[WARN] Error replacing & with &amp;" << std::endl;
    }
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
    uninstall_button.signal_clicked().connect(sigc::mem_fun(*this, &PluginInstance::uninstall));
    install_button.signal_clicked().connect(sigc::mem_fun(*this, &PluginInstance::download));
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

//Signal handler for the uninstall button
void PluginInstance::uninstall()
{
    std::thread uninstall_thread(&PluginInstance::uninstall_plugin, this);
    uninstall_thread.detach();
}

//Signal handler for the download button
void PluginInstance::download()
{
    std::thread download_thread(&PluginInstance::download_plugin, this);
    download_thread.detach();
}

//Download the plugin
void PluginInstance::download_plugin()
{
    get_spinner()->start();
    std::cout << "[INFO] Downloading plugin " << get_plugin_id().name << std::endl;
    //Raw plugin url
    std::string url = get_plugin_id().url;

    //Download the plugin
    Downloader::download(url, "download/" + get_plugin_id().name + ".zip", false);
    std::cout << "[INFO] Downloaded plugin " << get_plugin_id().name << std::endl;
    std::string home_folder;
     
    std::filesystem::create_directory("plugins/");
    std::string extract_command;
    std::string os = Functions::get_OS();
    std::string plugins_folder;
    std::string options;
    if(os == "Linux")
    {
        options = " -d ";
        home_folder = std::getenv("HOME");
        extract_command = "unzip -o \"";
        plugins_folder = home_folder + "/.local/share/endless-sky/plugins";
    }
    else if(os == "Windows")
    {
        options = " -o";
        
        home_folder = std::getenv("APPDATA");
        extract_command = "7za x -y \"";
        plugins_folder = home_folder + "\\endless-sky\\plugins";
    }
    else
    {
        options = " -d ";
        //Does this work for macOS? I don't have a mac to test it on (*cries in poor*)
        extract_command = "unzip -o \"";
        plugins_folder = home_folder + "/Library/Application Support/endless-sky/plugins/";
    }
    std::cout << extract_command + "download/" + get_plugin_id().name + ".zip\"" + options + "plugins/" << std::endl;
    system((extract_command + "download/" + get_plugin_id().name + ".zip\"" + options + "plugins/").c_str());
    std::filesystem::remove_all("download/" + get_plugin_id().name + ".zip");
    
    std::string folder = Functions::get_first_folder("plugins/");
    if(Functions::get_number_of_files_in_folder(folder) == 1)
    {
        folder = Functions::get_first_folder(folder);
    }
    std::filesystem::copy(folder, plugins_folder + "/" + get_plugin_id().name, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
    
    std::filesystem::remove_all("plugins/");

    get_spinner()->stop();
    //For some kind of reason, these dialogs completely freeze the program on windows
    if(os != "Windows")
    {
        InformationDialog d("Plugin installed", "The plugin has been installed successfully");
        d.show_all();
        d.run();
    }
}

void PluginInstance::uninstall_plugin()
{
    get_spinner()->start();
    std::string home_folder; 
    std::string os = Functions::get_OS();
    std::string plugins_folder;
    if(os == "Linux")
    {
        home_folder = std::getenv("HOME");
        plugins_folder = home_folder + "/.local/share/endless-sky/plugins";
    }
    else if(os == "Windows")
    {
        home_folder = std::getenv("APPDATA");
        plugins_folder = home_folder + "\\endless-sky\\plugins";
    }
    else
    {
        home_folder = std::getenv("HOME");
        plugins_folder = home_folder + "/Library/Application Support/endless-sky/plugins/";
    }
    std::filesystem::remove_all(plugins_folder + "/" + get_plugin_id().name);
    get_spinner()->stop();
    if(os != "Windows")
    {
        InformationDialog d("Plugin uninstalled", "The plugin has been uninstalled successfully");
        d.show_all();
        d.run();
    }

    set_installed(false);
}