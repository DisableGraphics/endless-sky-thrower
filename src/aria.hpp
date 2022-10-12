#pragma once
#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/progressbar.h"
#include <thread>
#define CURL_STATICLIB
#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include "global_variables.hpp"
#include "gtkmm/headerbar.h"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>
//Every 300 milliseconds, the progress bar will be updated. The progress bar would crash if done with less interval time
//on my third gen Intel shitty laptop
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL 300000

inline std::string get_OS()
{
    #ifdef _WIN32
        return "Windows";
    #elif __APPLE__ || __MACH__
        return "MacOS";
    #elif __linux__
        return "Linux";
    #else
        return "Other";
    #endif
}
typedef struct
{
    Gtk::ProgressBar *progress_bar;
    Gtk::Window * window;
} Tw;
//Used internally for the xferinfo function
struct myprogress {
  curl_off_t lastruntime; /* type depends on version, see above */
  CURL *curl;
  Tw tw;
};
//Will write the downloaded data to a file
inline size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}
//This function is used to update the progress bar. Will set the progress bar to a fraction of the download progress.
static int xferinfo(void *p,
                    curl_off_t dltotal, curl_off_t dlnow,
                    curl_off_t ultotal, curl_off_t ulnow)
{
    struct myprogress *myp = (struct myprogress *)p;
    CURL *curl = myp->curl;
    curl_off_t curtime = 0;
 
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME_T, &curtime);
 
    
    //Every 300 milliseconds, the progress bar will update. This is to prevent the program from crashing, since it
    //would update the progress bar too fast for my shitty laptop to process correctly.
    if((curtime - myp->lastruntime) >= MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL) 
    {
        myp->lastruntime = curtime;
        if(dlnow != 0 && dltotal != 0)
        {
            if(myp->tw.window->is_active())
            {
                myp->tw.progress_bar->set_fraction((double)dlnow / (double)dltotal);
            }
        }
    }
    return 0;
}
//This function returns the instance name without the first "v" in the version number
//Used (mostly) for windows, since the ES naming scheme sucks
inline std::string instance_version_minus_v(std::string instance_version)
{
    std::string version = instance_version;
    version.erase(0, 1);
    return version;
}
//This is a thread used to: download the instance and update the progress bar
inline void aria2Thread(Gtk::ProgressBar * progress_bar, std::string type, std::string instance_name, std::string instance_version, Gtk::Window * window, bool different_naming_scheme = false)
{
    while(global::lock)
    {
        //Wait until the lock is released
        std::chrono::milliseconds dura( 1000 );
        std::this_thread::sleep_for(dura);
    }
    if(!global::lock)
    {
        global::lock = true;
    }
    if(!std::filesystem::exists("download/" + instance_name))
    {
        std::filesystem::create_directory("download/" + instance_name);
    }

    CURL *curl;
    FILE *fp;
    CURLcode res;
    struct myprogress prog;

    std::string url;

    std::string os = get_OS();
    url = "https://github.com/endless-sky/endless-sky/releases/download/";

    std::string instance_v = instance_version;
    bool upper_case = false;
    if(type == "Continuous")
    {
        instance_v = "continuous";
        upper_case = true;
        
        different_naming_scheme = true;
        
    }
    std::string file_prefix;
    if(os == "Linux")
    {
        if(different_naming_scheme)
        {
            url += instance_v + "/endless-sky-x86_64-" + instance_v +".AppImage";
        }
        else 
        {
            url += instance_v + "/endless-sky-amd64-" + instance_v + ".AppImage";
        }
        file_prefix = "endless-sky.AppImage";
    }
    else if(os == "Windows")
    {
        std::string es{upper_case? "EndlessSky" : "endless-sky"};
        
        if(different_naming_scheme)
        {
            url += instance_v + "/" + es + "-win64-" + instance_v + ".zip";
        }
        else 
        {
            url += instance_v + "/" + es + "-win64-" + instance_version_minus_v(instance_v) + ".zip";
        }
        file_prefix = "EndlessSky-win64.zip";
    }
    else //MacOS
    {
        std::string es{upper_case? "EndlessSky" : "endless-sky"};
    
        url += instance_v + "/" + es + "-macos-" + instance_v + ".zip";
        file_prefix = "EndlessSky-macos.zip";
    }
    
    //I like that I can output the file to a specific filename, so I don't have to rename it later.
    std::string out_str = ("download/" + instance_name + "/" + file_prefix).c_str();
    char outfilename[FILENAME_MAX];
    strcpy(outfilename, out_str.c_str());
    //Begin the download
    curl = curl_easy_init();
    if (curl) 
    {
        prog.lastruntime = 0;
        prog.curl = curl;
        prog.tw.window = window;
        prog.tw.progress_bar = progress_bar;
        fp = fopen(outfilename,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        //The write_data function will write the downloaded data to a file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
        /* pass the struct pointer into the xferinfo function */
        //Note: xferinfo is used to update the progress bar
        curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        if(res != CURLE_OK)
        {
            fprintf(stderr, "%s\n", curl_easy_strerror(res));
        }

        res = curl_easy_perform(curl);
        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    std::ifstream verification;
    verification.open(outfilename);
    std::string contents;
    verification >> contents;
    verification.close();
    if(contents == "Not")
    {
        std::cout << "[WARN] Download failed due to incorrect naming scheme. Retrying..." << std::endl;
        //I need to redownload the file using another naming scheme
        global::lock = false;
        aria2Thread(progress_bar, type, instance_name, instance_version, window, true);
    }
    else
    {
        std::cout << "[INFO] Verified correctness of download. Proceeding with next operation." << std::endl;
    }
    //Note: Since continuous may have a different naming scheme, this needs to fire if the instance is continuous
    //Also: The naming scheme of continuous versions is regular, so I don't need to worry about misfires
    if(!different_naming_scheme || type == "Continuous")
    {
        //Dialog to notify the user that the download is complete
        Gtk::Dialog finished_downloading_dialog;
        Gtk::VBox finished_downloading_vbox;
        Gtk::HeaderBar finished_downloading_headerbar;

        finished_downloading_headerbar.set_title("Finished Downloading");
        finished_downloading_headerbar.set_show_close_button(true);
        finished_downloading_dialog.set_titlebar(finished_downloading_headerbar);

        finished_downloading_vbox.set_spacing(10);
        finished_downloading_vbox.set_margin_top(10);
        finished_downloading_vbox.set_margin_bottom(10);
        finished_downloading_vbox.set_valign(Gtk::ALIGN_CENTER);
        finished_downloading_vbox.set_halign(Gtk::ALIGN_CENTER);
        
        finished_downloading_dialog.get_action_area()->pack_start(finished_downloading_vbox);
        Gtk::Label finished_downloading_label;
        finished_downloading_label.set_valign(Gtk::ALIGN_CENTER); 
        finished_downloading_label.set_halign(Gtk::ALIGN_CENTER);
        finished_downloading_label.set_justify(Gtk::JUSTIFY_CENTER);
        finished_downloading_label.set_markup("Finished downloading instance <b>" + instance_name + "</b> of type <b>" + type + "</b>.\n\nYou can now close this window.");
        finished_downloading_vbox.pack_start(finished_downloading_label);
        
        Gtk::Button * finished_downloading_button = finished_downloading_dialog.add_button("Close", Gtk::RESPONSE_OK);
        finished_downloading_button->set_resize_mode(Gtk::RESIZE_PARENT);
        finished_downloading_button->get_parent()->remove(*finished_downloading_button);
        finished_downloading_vbox.pack_start(*finished_downloading_button);
        finished_downloading_vbox.set_hexpand();
        finished_downloading_vbox.set_halign(Gtk::ALIGN_CENTER);

        finished_downloading_dialog.show_all();

        switch(finished_downloading_dialog.run())
        {
            case(Gtk::RESPONSE_OK):
            {
                finished_downloading_dialog.close();
                break;
            }
            default:
            {
                finished_downloading_dialog.close();
                break;
            }
        }
        
        global::lock = false;
        while(!window->is_active())
        {
            //Wait for the window to be visible
            sleep(1);
        }
        progress_bar->set_fraction(0);
        
    }
}
inline void download_plugin_json()
{
    //Raw json url
    std::string url = "https://raw.githubusercontent.com/EndlessSkyCommunity/endless-sky-plugins/master/generated/plugins.json";

    //Download the json file
    CURL *curl;
    FILE *fp;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) 
    {
        fp = fopen("download/plugins.json","wb");
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
    nlohmann::json j;
    std::ifstream i("download/plugins.json");
    i >> j;
    std::string plugin_name;
    std::string plugin_version;
    std::string plugin_url;
    std::string plugin_description;
    std::string plugin_author;
    std::string plugin_homepage;
    std::string plugin_license;
    std::string plugin_short_description;
    
    //Populate the plugins vector
    for (auto& element : j)
    {
        plugin_name = element["name"];
        plugin_version = element["version"];
        plugin_url = element["url"];
        plugin_description = element["description"];
        plugin_short_description = element["shortDescription"];
        plugin_license = element["license"];
        plugin_author = element["authors"];
        plugin_homepage = element["homepage"];
        global::plugins.push_back({plugin_name, plugin_author, plugin_version, plugin_description, plugin_short_description, plugin_homepage, plugin_license, plugin_url});
    }
    
}