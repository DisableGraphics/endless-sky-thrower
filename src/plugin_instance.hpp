#include "global_variables.hpp"
#include <filesystem>
#include <gtkmm.h>
#include "global_variables.hpp"
#include "gtkmm/dialog.h"
#include "gtkmm/enums.h"
#include "gtkmm/headerbar.h"
#include "gtkmm/hvbox.h"
#include <curl/curl.h>
#include "aria.hpp"
#include "gtkmm/label.h"
#include "gtkmm/separator.h"
#include <system_error>
#include <thread>
#include "functions.hpp"

class PluginInstance : public Gtk::VBox
{
    public:
        PluginInstance(Plugin_ID id, bool is_installed);
        Plugin_ID get_plugin_id()
        {
            return plugin_id;
        }
        void set_installed();
        void set_uninstalled();
        Gtk::Spinner * get_spinner()
        {
            return &spinner;
        }
    private:
        Plugin_ID plugin_id;
        Gtk::Label name_label;
        Gtk::Label version_label;
        Gtk::Label author_label;
        Gtk::Label description_label;
        Gtk::Label license_label;
        Gtk::Label homepage_label;
        Gtk::Button install_button;
        Gtk::Button uninstall_button;
        Gtk::Separator separator;
        Gtk::Spinner spinner;
        Gtk::Separator separator2;
};
//Download the plugin
inline void download_plugin(PluginInstance * plugin_id)
{
    plugin_id->get_spinner()->start();
    //Raw plugin url
    std::string url = plugin_id->get_plugin_id().url;

    //Download the plugin
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) 
    {
        fp = fopen(("download/" + plugin_id->get_plugin_id().name + ".zip").c_str(),"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        //The write_data function will write the downloaded data to a file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    std::string home_folder = std::getenv("HOME");
    std::filesystem::create_directory("plugins/");
    std::string extract_command;
    std::string os = get_OS();
    std::string plugins_folder;
    if(os == "Linux")
    {
        extract_command = "unzip -o \"";
        plugins_folder = home_folder + "/.local/share/endless-sky/plugins";
    }
    else if(os == "Windows")
    {
        extract_command = "7z x \"";
        plugins_folder = "%AppData%\\endless-sky\\plugins";
    }
    else
    {
        //Does this work for macOS? I don't have a mac to test it on (*cries in poor*)
        extract_command = "unzip -o \"";
        plugins_folder = home_folder + "/Library/Application Support/endless-sky/plugins/";
    }
    system((extract_command + "download/" + plugin_id->get_plugin_id().name + ".zip\" -d plugins/").c_str());
    std::filesystem::remove_all("download/" + plugin_id->get_plugin_id().name + ".zip");
    
    std::string folder = get_first_folder("plugins/");
    if(get_number_of_files_in_folder(folder) == 1)
    {
        folder = get_first_folder(folder);
    }
    std::filesystem::copy(folder, plugins_folder + "/" + plugin_id->get_plugin_id().name, std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing);
    
    std::filesystem::remove_all("plugins/");

    plugin_id->get_spinner()->stop();
    Gtk::HeaderBar header_bar;
    Gtk::Dialog dialog;
    dialog.set_titlebar(header_bar);
    header_bar.set_title("Plugin installed");
    header_bar.set_show_close_button();
    dialog.get_action_area()->set_orientation(Gtk::ORIENTATION_VERTICAL);
    Gtk::Image image;
    image.set_from_icon_name("dialog-information", Gtk::ICON_SIZE_DIALOG);
    dialog.get_action_area()->pack_start(image);
    Gtk::Label label;
    label.set_markup("The plugin <b>" + plugin_id->get_plugin_id().name + "</b> has been installed.\n\nYou can now close this window.");
    
    label.set_justify(Gtk::JUSTIFY_CENTER);

    dialog.get_action_area()->pack_start(label);
    dialog.show_all();
    dialog.run();
}

inline void uninstall_plugin(PluginInstance * plugin_id)
{
    plugin_id->get_spinner()->start();
    std::string home_folder = std::getenv("HOME");
    std::string os = get_OS();
    std::string plugins_folder;
    if(os == "Linux")
    {
        plugins_folder = home_folder + "/.local/share/endless-sky/plugins";
    }
    else if(os == "Windows")
    {
        plugins_folder = "%AppData%\\endless-sky\\plugins";
    }
    else
    {
        plugins_folder = home_folder + "/Library/Application Support/endless-sky/plugins/";
    }
    std::filesystem::remove_all(plugins_folder + "/" + plugin_id->get_plugin_id().name);
    plugin_id->get_spinner()->stop();
    
    Gtk::Dialog dialog("Plugin uninstalled");
    dialog.get_action_area()->pack_start(*Gtk::manage(new Gtk::Image(Gtk::Stock::DIALOG_INFO, Gtk::ICON_SIZE_DIALOG)));
    Gtk::HeaderBar header_bar;
    header_bar.set_title("Plugin uninstalled");
    dialog.set_titlebar(header_bar);
    header_bar.set_show_close_button();
    //This label should be at bottom
    dialog.get_action_area()->set_orientation(Gtk::ORIENTATION_VERTICAL);
    Gtk::Label label;
    label.set_markup("The plugin <b>" + plugin_id->get_plugin_id().name + "</b> has been uninstalled.\n\nYou can now close this window.");
    label.set_justify(Gtk::JUSTIFY_CENTER);

    dialog.set_halign(Gtk::ALIGN_CENTER);
    dialog.set_valign(Gtk::ALIGN_CENTER);

    dialog.get_action_area()->pack_start(label);
    dialog.show_all();
    dialog.run();

    plugin_id->set_uninstalled();
}

inline void PluginInstance::set_installed()
{
    install_button.set_label("Update");
    pack_start(uninstall_button);
    show_all();
}

inline void PluginInstance::set_uninstalled()
{
    install_button.set_label("Install");
    remove(uninstall_button);
    show_all();
}

inline PluginInstance::PluginInstance(Plugin_ID id, bool is_installed)
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