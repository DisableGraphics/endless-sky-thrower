#pragma once
#include "gtkmm/progressbar.h"
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
        return "Mac OSX";
    #elif __linux__
        return "Linux";
    #else
        return "Other";
    #endif
}
//Used internally for the xferinfo function
struct myprogress {
  curl_off_t lastruntime; /* type depends on version, see above */
  CURL *curl;
  Gtk::ProgressBar * progr;
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
            myp->progr->set_fraction((double)dlnow / (double)dltotal);
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
inline void aria2Thread(Gtk::ProgressBar * progress_bar, std::string type, std::string instance_name, std::string instance_version)
{
    if(!std::filesystem::exists("download/" + instance_name))
    {
        std::filesystem::create_directory("download/" + instance_name);
    }

    CURL *curl;
    FILE *fp;
    CURLcode res;
    struct myprogress prog;

    char url[4096];
    if(type == "Continuous")
    {
        if(get_OS() == "Linux")
        {
            strcpy(url, "https://github.com/endless-sky/endless-sky/releases/download/continuous/endless-sky-x86_64-continuous.AppImage");
        }
        else if(get_OS() == "Windows")
        {
            strcpy(url, "https://github.com/endless-sky/endless-sky/releases/download/continuous/EndlessSky-win64-continuous.zip");
        }
        else if(get_OS() == "Mac OSX")
        {
            strcpy(url, "https://github.com/endless-sky/endless-sky/releases/download/continuous/EndlessSky-macOS-continuous.zip");
        }
    }
    else if(type == "Stable")
    {
        if(get_OS() == "Linux")
        {
            strcpy(url, ("https://github.com/endless-sky/endless-sky/releases/download/" + instance_version + "/endless-sky-amd64-" + instance_version + ".AppImage").c_str());
        }
        else if(get_OS() == "Windows")
        {
            //<irony>Courtesy of the great naming scheme of the endless sky developers!</irony> 
            //(Yes, I'm fluent in HTML but who doesn't at this point in time)
            strcpy(url, ("https://github.com/endless-sky/endless-sky/releases/download/" + instance_version + "/endless-sky-win64-" + instance_version_minus_v(instance_version) + ".zip").c_str());
        }
        else if(get_OS() == "Mac OS")
        {
            strcpy(url, ("https://github.com/endless-sky/endless-sky/releases/download/" + instance_version + "/endless-sky-macos-" + instance_version + ".zip").c_str());
        }
    }
    std::string file_prefix;
    if(get_OS() == "Linux")
    {
        file_prefix = "endless-sky.AppImage";
    }
    else if(get_OS() == "Windows")
    {
        file_prefix = "EndlessSky-win64.zip";
    }
    else if(get_OS() == "Mac OS")
    {
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
        prog.progr = progress_bar;
        fp = fopen(outfilename,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
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

    double progress = 0;
    int count{0};
    for(;;) 
    {
        count++;
        
        if( count != 1) 
        {
            break;
        }

    }
    //Dialog to notify the user that the download is complete
    Gtk::Dialog finished_downloading_dialog;
    Gtk::VBox finished_downloading_vbox;
    Gtk::HeaderBar finished_downloading_headerbar;

    finished_downloading_headerbar.set_title("Finished Downloading");
    finished_downloading_headerbar.set_show_close_button(true);
    finished_downloading_dialog.set_titlebar(finished_downloading_headerbar);
    
    finished_downloading_dialog.get_action_area()->pack_start(finished_downloading_vbox);
    finished_downloading_vbox.pack_start(*Gtk::manage(new Gtk::Label("Finished downloading " + type + " build.")));
    
    Gtk::Button b ("OK");
    b.signal_clicked().connect([&finished_downloading_dialog](){finished_downloading_dialog.close();});
    finished_downloading_vbox.pack_start(b);
    finished_downloading_dialog.show_all();

    if(finished_downloading_dialog.run() == Gtk::RESPONSE_OK)
    {
        finished_downloading_dialog.close();
    }
    progress_bar->set_fraction(0);
    global::lock = false;
}
inline void download_plugin_json()
{
    //Raw yaml url
    std::string url = "https://raw.githubusercontent.com/EndlessSkyCommunity/endless-sky-plugins/master/generated/plugins.json";

    //Download the yaml file
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