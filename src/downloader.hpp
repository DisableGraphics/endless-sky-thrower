#pragma once
#include "gtkmm/progressbar.h"
#include <cstddef>
#include <thread>
#define CURL_STATICLIB
#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <fstream>

//Every 300 milliseconds, the progress bar will be updated. The progress bar would crash if done with less interval time
//on my third gen Intel shitty laptop
#define MINIMAL_PROGRESS_FUNCTIONALITY_INTERVAL 300000

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

class Downloader
{
    public:
        Downloader();
        //Downloads the instance
        void download_instance(Gtk::ProgressBar * progress_bar, std::string type, std::string instance_name, std::string instance_version, Gtk::Window * window);
        static void download_plugin_json();
        static size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);
        static void download(std::string url, std::string file_name, bool custom_user_agent);
        
    private:
        static int xferinfo(void *p, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow);
        std::string get_url(std::string instance_type, std::string instance_version);
        std::string get_response_from_api(std::string release_id);
        std::string get_release_id(std::string instance_type, std::string instance_version);
        std::string gen_file_prefix();
        
};


