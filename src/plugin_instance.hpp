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
        void uninstall_plugin();
        
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
        void uninstall();
};