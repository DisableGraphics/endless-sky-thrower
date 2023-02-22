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
        
        void download_plugin();
        void uninstall_plugin();
        
        //Getters
        Plugin_ID get_plugin_id();
        Gtk::Spinner * get_spinner();
        
        void set_installed(bool installed = true);

        
        
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
        Gtk::Separator separator2;

        Gtk::Spinner spinner;
        
        void download();
        void uninstall();
};