#include "downloader.hpp"
#include "gtkmm/progressbar.h"
#include "nlohmann/json_fwd.hpp"
#include "secondary_dialogs.hpp"
#include "global_variables.hpp"
#include "secondary_dialogs.hpp"
#include "functions.hpp"
#include <cstddef>
#include <cstdio>
#include <curl/curl.h>
#include <curl/easy.h>

Downloader::Downloader()
{
    //nothing here
}
//Will write the downloaded data to a file
size_t Downloader::write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

//This function is used to update the progress bar. Will set the progress bar to a fraction of the download progress.
int Downloader::xferinfo(void *p,
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

//Downloads and populates the list of plugins
void Downloader::download_plugin_json()
{
    //Raw json url
    std::string url = "https://raw.githubusercontent.com/EndlessSkyCommunity/endless-sky-plugins/master/generated/plugins.json";

    download(url, "download/plugins.json", false);
    //Get the size of the json file to test if it was downloaded correctly. If it is 0, then the download failed. (No internet)
    std::ifstream verification;
    verification.open("download/plugins.json");
    verification.seekg(0, std::ios::end);
    int size = verification.tellg();
    verification.close();

    if(size == 0)
    {
        std::cout << "[WARN] Download of plugins.json file failed. If there's an internet connection, please try again" << std::endl;
        return;
    }
    else
    {
        std::cout << "[INFO] Verified download. Proceeding with next operation." << std::endl;
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

//This is a thread used to: download the instance and update the progress bar
void Downloader::download_instance(Gtk::ProgressBar * progress_bar, std::string type, std::string instance_name, std::string instance_version, Gtk::Window * window)
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

    std::string url{get_url(type, instance_version)};
    std::cout << "[INFO] Downloading " << instance_name << " version " << instance_version << std::endl;
    std::cout << "[INFO] Downloading from " << url << std::endl;
    if(url == "None")
    {
        InformationDialog warns("Error",
        (std::string)"No instance found with that name and version.\nPlease check for any typos in the name or release number and try again\n",
        true);
        warns.run();
        global::lock = false;
        return;
    }
    else if(url == "Rate Limited")
    {
        InformationDialog warns("Error",
        (std::string)"You have been rate limited by GitHub. Please wait a few minutes and try again.\n",
        true);
        warns.run();
        global::lock = false;
        return;
    }

    
    std::string file_prefix = gen_file_prefix();
    //I like that I can output the file to a specific filename, so I don't have to rename it later.
    std::string out_str = ("download/" + instance_name + "/" + file_prefix);
    
    download(url, out_str, false, true, progress_bar, window);
    std::ifstream verification;
    verification.open(out_str);
    std::string contents;
    verification >> contents;
    verification.close();
    
    //Dialog to notify the user that the download is complete
    //This dialog bugs on windows, so I'm disabling it for now
    #ifndef _WIN32
    InformationDialog * dialog = new InformationDialog("Download Complete", "The instance has been downloaded correctly. You can now launch the game.");
    dialog->show_all();
    dialog->run();
    #endif

    global::lock = false;

    while(!window->is_active())
    {
        //Wait for the window to be visible
        sleep(1);
    }
    progress_bar->set_fraction(0);
    #ifndef _WIN32
    delete dialog;
    #endif
    
}

std::string Downloader::get_release_id(std::string instance_type, std::string instance_version)
{
    //just download a json file with all the releases and find the one that matches the version
    std::string url = "https://api.github.com/repos/endless-sky/endless-sky/releases";
    std::string response{""};
    download_buffered(url, response, true);
    
    //Parse the json file
    nlohmann::json j = nlohmann::json::parse(response);
    int release_id{-1};
    if(response.find("API rate limit exceeded") != std::string::npos)
    {
        return "Rate Limited";
    }
    try
    {
        for(auto& element : j)
        {
            if(element["tag_name"] == instance_version)
            {
                release_id = element["id"];
                break;
            }
        }
    } 
    catch(std::exception& e)
    {
        std::cout << "[ERROR] " << e.what() << std::endl;
        std::cout << "[ERROR] Could not find Release ID." << std::endl;
        std::cout << "[ERROR] Have you been rate limited?" << std::endl;

        //check if the user has been rate limited
        if(response.find("API rate limit exceeded") != std::string::npos)
        {
            return "Rate Limited";
        }
    }
    
    //std::filesystem::remove("download/releases.json");
    std::cout << "[INFO] Release ID: " << release_id << std::endl;
    return release_id == -1 ? "None" : std::to_string(release_id);
}

std::string Downloader::get_response_from_api(std::string release_id)
{
    if(release_id == "None")
    {
        return "None";
    }
    else if(release_id == "Rate Limited")
    {
        return "Rate Limited";
    }
    std::string url = "https://api.github.com/repos/endless-sky/endless-sky/releases/" + release_id + "/assets";
    std::string response{""};
    download_buffered(url, response, true);
    
    if(response.find("API rate limit exceeded") != std::string::npos)
    {
        return "Rate Limited";
    }

    return response;
}

std::string Downloader::get_url(std::string instance_type, std::string instance_version)
{
    //Call the GitHub API to get the download URL
    if(instance_type == "Continuous")
    {
        instance_version = "continuous";
    }
    else
    {
        if(!Functions::has_v(instance_version))
        {
            instance_version = "v" + instance_version;
        }
    }
    std::string url = "https://api.github.com/repos/endless-sky/endless-sky/releases/tags/" + instance_version;
    std::string os = Functions::get_OS();
    //Get all the release assets for the given release and find the one that matches the OS
    std::string response_str = get_response_from_api(get_release_id(instance_type, instance_version));
    if(response_str == "None")
    {
        return "None";
    }
    else if(response_str == "Rate Limited")
    {
        return "Rate Limited";
    }
    nlohmann::json response{nlohmann::json::parse(response_str)};
    std::string download_url{"None"};
    std::string searchfor{""};
    if(os == "Windows")
    {
        searchfor = "win64";
    }
    else if(os == "Linux")
    {
        searchfor = "AppImage";
    }
    else if(os == "Mac")
    {
        searchfor = "mac";
    }

    try{
        for(auto& element : response)
        {
            for(auto& asset : element)
            {
                if(asset["name"].dump().find(searchfor) != std::string::npos)
                {
                    download_url = asset["browser_download_url"];
                    std::cout << "[INFO] Download URL: " << download_url << std::endl;
                    break;
                }
            }
        }
    } catch(std::exception& e)
    {
        std::cout << "[ERROR] " << e.what() << std::endl;
        std::cout << "[ERROR] Could not find download URL." << std::endl;
        std::cout << "[ERROR] Have you been rate limited?" << std::endl;
    }
    std::cout << "[INFO] Download URL: " << download_url << std::endl;
    return download_url;
}

void Downloader::download(const std::string &url, std::string file_name, bool custom_user_agent, bool xferinfo_callback, Gtk::ProgressBar * progress_bar, Gtk::Window * win)
{
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) 
    {
        FILE *fp;
        fp = fopen(file_name.c_str(),"wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if(custom_user_agent)
        {
            curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "application/vnd.github+json");
            curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
        }
        if(xferinfo_callback)
        {
            struct myprogress prog;
            prog.lastruntime = 0;
            prog.curl = curl;
            prog.tw.window = win;
            prog.tw.progress_bar = progress_bar;
            curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
            curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
            curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
        }
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        /* always cleanup */
        curl_easy_cleanup(curl);
        if(res != CURLE_OK)
        {
            std::cout << "[ERROR] " << curl_easy_strerror(res) << std::endl;
        }
        fclose(fp);
    }
    
}

size_t Downloader::write_data_buffer(void *ptr, size_t size, size_t nmemb, std::string *stream)
{
    int dataLength = size * nmemb;
    stream->append((char*)ptr, dataLength);
    return dataLength;
}

void Downloader::download_buffered(const std::string &url, std::string &buffer, bool custom_user_agent)
{
    CURL *curl;

    CURLcode res;
    curl = curl_easy_init();
    if (curl) 
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if(custom_user_agent)
        {
            curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "application/vnd.github+json");
            curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent.c_str());
        }
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_buffer);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
        {
            std::cout << "[ERROR] " << curl_easy_strerror(res) << std::endl;
        }
        curl_easy_cleanup(curl);
    }
}

std::string Downloader::gen_file_prefix()
{
    std::string os = Functions::get_OS();
    std::string file_prefix;
    if(os == "Windows")
    {
        file_prefix = "endless-sky-win64.zip";
    }
    else if(os == "Linux")
    {
        file_prefix = "endless-sky.AppImage";
    }
    else if(os == "Mac")
    {
        file_prefix = "endless-sky-mac.zip";
    }
    return file_prefix;
}