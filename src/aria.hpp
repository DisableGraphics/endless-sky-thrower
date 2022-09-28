#pragma once
#include "gtkmm/progressbar.h"
#define CURL_STATICLIB
#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include "global_variables.hpp"
#include "gtkmm/headerbar.h"
#include <curl/curl.h>
//Makes the download progress bar look like shit, but this works. 100000 is too low, so the program crashes. Wil take a look at it in a really shitty PC.
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
struct myprogress {
  curl_off_t lastruntime; /* type depends on version, see above */
  CURL *curl;
  Gtk::ProgressBar * progr;
};

inline size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

static int xferinfo(void *p,
                    curl_off_t dltotal, curl_off_t dlnow,
                    curl_off_t ultotal, curl_off_t ulnow)
{
    struct myprogress *myp = (struct myprogress *)p;
    CURL *curl = myp->curl;
    curl_off_t curtime = 0;
 
    curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME_T, &curtime);
 
    /* under certain circumstances it may be desirable for certain functionality
        to only run every N seconds, in order to do this the transaction time can
        be used */
    //I think that the progress bar is updated too fast for slow computers to process. It updates every 300 millisecons, but
    //The look and feel is far from optimal
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
inline std::string instance_version_minus_v(std::string instance_version)
{
    std::string version = instance_version;
    version.erase(0, 1);
    return version;
}
//TODO: Determine what's crashing this function in low-end systems. Won't crash on a system with 16 Gb RAM, Ryzen 7 5800H
//and Nvidia RTX 3050 Ti, but will crash on a system with 8 Gb RAM, Intel Core i5 3360M and Intel HD Graphics 4000.
//The gap is too high to be a memory issue, so it's probably a CPU issue.

//When downloading, the progress bar freezes and crashes the program.
//I think the problem lies on the xferinfo function, but I'm not sure.
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
            //Courtesy of the great naming scheme of the endless sky developers!
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
    std::string out_str = ("download/" + instance_name + "/" + file_prefix).c_str();
    char outfilename[FILENAME_MAX];
    strcpy(outfilename, out_str.c_str());
    curl = curl_easy_init();
    if (curl) 
    {
        prog.lastruntime = 0;
        prog.curl = curl;
        prog.progr = progress_bar;
        fp = fopen(outfilename,"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
        /* pass the struct pointer into the xferinfo function */
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

    if(type == "Continuous")
    {
    }
    else if(type == "Stable")
    {
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
    
    //std::filesystem::rename("endless-sky-x86_64-continuous.AppImage", (std::string)"download/" + instance_name + "/endless-sky-x86_64-continuous.AppImage");
    //progress_bar->set_text("Download Complete");
    progress_bar->set_fraction(0);
    global::lock = false;
}