#include "global_variables.hpp"
#include <filesystem>
#include <gtkmm.h>
#include <curl/curl.h>
#include <memory>
#include <system_error>
#include <thread>
#include "functions.hpp"
#include "secondary_dialogs.hpp"

class PluginInstance : public Gtk::VBox
{
    public:
        PluginInstance(Plugin_ID id, bool is_installed);
        //Getters
        Plugin_ID get_plugin_id();
        Gtk::Spinner * get_spinner();
        
        void set_installed(bool installed = true);
        void download_plugin();
        
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
        void download();
};

inline void uninstall_plugin(PluginInstance * plugin_id)
{
    plugin_id->get_spinner()->start();
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
    std::filesystem::remove_all(plugins_folder + "/" + plugin_id->get_plugin_id().name);
    plugin_id->get_spinner()->stop();
    if(os != "Windows")
    {
        InformationDialog d("Plugin uninstalled", "The plugin has been uninstalled successfully");
        d.show_all();
        d.run();
    }

    plugin_id->set_installed(false);
}