#include "global_variables.hpp"
#include <filesystem>
#include <gtkmm.h>
#include "global_variables.hpp"
#include "gtkmm/enums.h"
#include "gtkmm/hvbox.h"
#include <curl/curl.h>
#include "aria.hpp"
#include "gtkmm/separator.h"
#include <thread>

inline void download_plugin(Plugin_ID plugin_id)
{
    //Raw plugin url
    std::string url = plugin_id.url;

    //Download the plugin
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) 
    {
        fp = fopen(("download/" + plugin_id.name + ".zip").c_str(),"wb");
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
    
    std::filesystem::create_directory("plugins/");
    
    if(get_OS() == "Linux")
    {
        //Extract the plugin
        system(("unzip -o \"download/" + plugin_id.name + ".zip\" -d \"plugins/" + plugin_id.name + "\"").c_str());

        //Delete the zip file
        std::filesystem::remove_all("download/" + plugin_id.name + ".zip");

        //Move the plugin to the plugins folder
        system(("mv \"plugins/" + plugin_id.name + "/\" ~/.local/share/endless-sky/plugins").c_str());
    }
    else if(get_OS() == "Windows")
    {
        //Extract the plugin
        system(("7za download/" + plugin_id.name + ".zip -d plugins/" + plugin_id.name).c_str());

        //Delete the zip file
        std::filesystem::remove_all("download/" + plugin_id.name + ".zip");

        //Move the plugin to the plugins folder
        system(("move plugins/" + plugin_id.name + "/* %AppData%\\endless-sky\\plugins" + plugin_id.name).c_str());

    }
    else if(get_OS() == "MacOS")
    {
        //Extract the plugin
        system(("unzip download/" + plugin_id.name + ".zip -d plugins/" + plugin_id.name).c_str());

        //Delete the zip file
        std::filesystem::remove_all("download/" + plugin_id.name + ".zip");

        //Move the plugin to the plugins folder
        system(("mv plugins/" + plugin_id.name + "/* ~/Library/Application Support/endless-sky/plugins/" + plugin_id.name).c_str());
    }
    std::filesystem::remove_all("plugins/");
}

class PluginInstance : public Gtk::VBox
{
    public:
        PluginInstance(Plugin_ID id)
        {
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

            install_button.set_label("Download");

            pack_start(name_label);
            pack_start(version_label);

            pack_start(author_label);
            pack_start(description_label);
            pack_start(install_button);

            install_button.signal_clicked().connect([&](){std::thread t(std::bind(download_plugin, plugin_id)); t.detach();});

            separator.set_margin_top(10);
            //The last one is the separators
            pack_end(separator);
            show_all();
        }
        Plugin_ID get_plugin_id()
        {
            return plugin_id;
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
        Gtk::Separator separator;

        
        
};