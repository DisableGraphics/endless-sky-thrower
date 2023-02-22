#include "secondary_dialogs.hpp"

InformationDialog::InformationDialog(std::string title, std::string message, bool warning)
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

void InformationDialog::quit()
{
    hide();
}

DeletingInstanceDialog::DeletingInstanceDialog(std::string message)
{
    set_titlebar(title_bar);
    title_bar.set_title("Deleting instance");
    title_bar.set_show_close_button();
    resize(200, 100);
    set_modal(true);
    set_transient_for(*this);
    set_position(Gtk::WIN_POS_CENTER_ALWAYS);

    label.set_markup(message);
    label.set_justify(Gtk::JUSTIFY_CENTER);
    label.set_line_wrap(true);
    cancel_button.set_label("Cancel");
    ok_button.set_label("OK");
    icon.set_from_icon_name("dialog-warning", Gtk::ICON_SIZE_DIALOG);
    get_content_area()->pack_start(icon);
    get_content_area()->pack_start(label);
    get_content_area()->pack_start(buttons_box);
    buttons_box.pack_start(cancel_button);
    buttons_box.pack_start(ok_button);

    cancel_button.signal_clicked().connect(sigc::mem_fun(*this, &DeletingInstanceDialog::on_cancel));
    ok_button.signal_clicked().connect(sigc::mem_fun(*this, &DeletingInstanceDialog::on_ok));
    
    show_all_children();
}

void DeletingInstanceDialog::on_cancel()
{
    cancel = true;
    hide();
}

void DeletingInstanceDialog::on_ok()
{
    cancel = false;
    hide();
}

bool DeletingInstanceDialog::cancelled()
{
    return cancel;
}