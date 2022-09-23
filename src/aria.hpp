#pragma once
#include <iostream>
#include <gtkmm.h>
#include <filesystem>
#include "global_variables.hpp"
#include "gtkmm/headerbar.h"
#ifdef __linux__
#include <aria2/aria2.h>
#endif
#ifdef __linux__
inline int downloadEventCallback(aria2::Session* session, aria2::DownloadEvent event,
                          unsigned long gid, void* userData)
{
    switch(event) {
    case aria2::EVENT_ON_DOWNLOAD_COMPLETE:
        std::cerr << "Completed downloading continuous build"  << std::endl;
        break;
    case aria2::EVENT_ON_DOWNLOAD_ERROR:
        std::cerr << "ERROR";
        break;
    default:
        return 0;
    }
    return 0;
}
#endif
inline void aria2Thread(Gtk::ProgressBar * prog, std::string type, std::string instance_name)
{
    #ifdef __linux__
    aria2::Session* session;
    // Create default configuration. The libaria2 takes care of signal
    // handling.
    aria2::SessionConfig config;
    // Add event callback
    config.downloadEventCallback = downloadEventCallback;
    
    session = aria2::sessionNew(aria2::KeyVals(), config);
    aria2::KeyVals options;

    options.push_back(std::pair<std::string, std::string> ("file-allocation", "none"));
    auto rv = aria2::addUri(session, nullptr, {"https://github.com/endless-sky/endless-sky/releases/download/continuous/endless-sky-x86_64-continuous.AppImage"}, options);
    double progress = 0;
    int count{0};
    for(;;) 
    {
        count++;
        auto rv = aria2::run(session, aria2::RUN_ONCE);
        if(rv != 1) 
        {
            break;
        }
        
        
        if(count >= 10)
        {
            std::vector<aria2::A2Gid> gids = aria2::getActiveDownload(session);
            for(const auto& gid : gids) 
            {
                aria2::DownloadHandle* dh = aria2::getDownloadHandle(session, gid);
                if(dh) 
                {
                    progress = (double)dh->getCompletedLength() / (double)dh->getTotalLength();
                    std::cout << progress << std::endl;
                    if(dh->getTotalLength() == 0) 
                    {
                        progress = 0;
                    }
                    
                    
                    aria2::deleteDownloadHandle(dh);
                    prog->set_text(std::to_string(progress * 100) + "%");
                    prog->set_fraction(progress);
                }
            }
            count = 0;
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
    if(!std::filesystem::exists("download/" + instance_name))
    {
        std::filesystem::create_directory("download/" + instance_name);
    }
    std::filesystem::rename("endless-sky-x86_64-continuous.AppImage", (std::string)"download/" + instance_name + "/endless-sky-x86_64-continuous.AppImage");
    aria2::sessionFinal(session);
    prog->set_text("Download Complete");
    prog->set_fraction(0);
    global::lock = false;

    #elif _WIN32
    std::string url = "https://github.com/endless-sky/endless-sky/releases/download/continuous/endless-sky-win64-continuous.zip";
    system(("wget " + url + " -o download/endless-sky-win64-continuous.zip").c_str());
    global::lock = false;

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

    #endif
}