#pragma once
//This is where information dialogs are
#include "gtkmm/headerbar.h"
#include <gtkmm.h>
#include <iostream>

class InformationDialog : public Gtk::Dialog
{
    public:
        InformationDialog(std::string title, std::string message, bool warning = false);
        
    private:
        Gtk::Label message_label;
        Gtk::Button ok_button;
        Gtk::HeaderBar title_bar;
        Gtk::Image icon;

        void quit();
        
};

class DeletingInstanceDialog : public Gtk::Dialog
{
    public:
        DeletingInstanceDialog(std::string message);

    private:
        Gtk::Label label;
        
        Gtk::Button cancel_button;
        Gtk::Button ok_button;
        Gtk::HeaderBar title_bar;
        Gtk::Image icon;
        bool cancel{false};
        
};