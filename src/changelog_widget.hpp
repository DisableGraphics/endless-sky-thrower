#pragma once
#include <climits>
#include <gtkmm.h>
#include <iostream>
#include <filesystem>
#include <curl/curl.h>

class ChangelogWidget : public Gtk::VBox
{
    public:
        ChangelogWidget();
        void download_changelog();
        void set_changelog();
    private:
        Gtk::Label changelog_label;
        Gtk::ScrolledWindow scrolled_window;
        Gtk::TextView changelog_textview;
        Gtk::Separator separator;
        std::string changelog;
        std::string files_dir = "download/";
};