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
        //This is the cutest function I've ever wrote lol
        void quit()
        {
            hide();
        }
};

inline InformationDialog::InformationDialog(std::string title, std::string message, bool warning)
{
    set_titlebar(title_bar);
    title_bar.set_title(title);
    title_bar.set_show_close_button();
    resize(100, 100);
    set_modal(true);
    set_transient_for(*this);
    set_position(Gtk::WIN_POS_CENTER_ALWAYS);

    message_label.set_markup(message);
    message_label.set_justify(Gtk::JUSTIFY_CENTER);
    ok_button.set_label("OK");
    ok_button.signal_clicked().connect(sigc::mem_fun(*this, &InformationDialog::quit));
    icon.set_from_icon_name("dialog-information", Gtk::ICON_SIZE_DIALOG);
    if(warning)
    {
        icon.set_from_icon_name("dialog-warning", Gtk::ICON_SIZE_DIALOG);
    }
    get_content_area()->pack_start(icon);
    get_content_area()->pack_start(message_label);
    get_content_area()->pack_start(ok_button);
    
    show_all_children();
}    