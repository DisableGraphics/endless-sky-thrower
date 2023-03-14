#include "dialogs.hpp"
#include "gtkmm/hvbox.h"

NewInstanceDialog::NewInstanceDialog()
{
    set_titlebar(header);
    header.set_show_close_button();
    header.set_title("New Instance");
    header.set_subtitle("Create a new Endless Sky instance");
    name_entry.set_placeholder_text("Instance Name");
    browse_button.set_image_from_icon_name("folder-symbolic");
    
    get_content_area()->pack_start(name_entry); 
    get_content_area()->pack_start(instance_type_combo);
    get_content_area()->pack_start(version_entry);
    version_entry.set_placeholder_text("Version");
    get_content_area()->pack_start(buttons_box);

    instance_type_combo.append("Stable release");
    instance_type_combo.append("Continuous build");
    instance_type_combo.append("Custom instance");
    instance_type_combo.append("PR");
    instance_type_combo.set_active(0);

    autoupdate.set_label("Autoupdate");
    autoupdate.set_tooltip_text("Updates the instance\nwhen ESThrower is opened");

    get_content_area()->pack_start(untouched);
    untouched.set_label("Vanilla");
    untouched.set_tooltip_text("This instance will not have any plugins");
        
    buttons_box.pack_start(cancel_button);
    
    cancel_button.signal_clicked().connect(sigc::mem_fun(*this, &NewInstanceDialog::on_cancel_button_clicked));
    instance_type_combo.signal_changed().connect(sigc::mem_fun(*this, &NewInstanceDialog::on_combo_changed));
    browse_button.signal_clicked().connect(sigc::mem_fun(*this, &NewInstanceDialog::on_browse_button_clicked));
    show_all();      
}

bool NewInstanceDialog::auto_update()
{
    return autoupdate.get_active();
}

bool NewInstanceDialog::vanilla()
{
    return untouched.get_active();
}

int NewInstanceDialog::get_selected()
{
    return instance_type_combo.get_active_row_number();
}

Gtk::HBox * NewInstanceDialog::get_buttons_box()
{
    return &buttons_box;
}

std::string NewInstanceDialog::get_naem()
{
    return name_entry.get_text();
}

std::string NewInstanceDialog::get_version()
{
    return get_selected() != 1? version_entry.get_text() : "0";
}

std::string NewInstanceDialog::get_typee()
{
    return get_selected() == 0?"Stable":get_selected() == 1?"Continuous":"Custom";
}

void NewInstanceDialog::on_cancel_button_clicked()
{
    close();
}

void NewInstanceDialog::on_browse_button_clicked()
{
    Gtk::FileChooserDialog dialog("Please select an endless sky executable", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_transient_for(*this);
    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
    dialog.add_button("_Open", Gtk::RESPONSE_OK);
    int result = dialog.run();
    switch(result)
    {
        case Gtk::RESPONSE_OK:
            version_entry.set_text(dialog.get_filename());
            break;
        case Gtk::RESPONSE_CANCEL:
            std::cout << "Cancel clicked." << std::endl;
            break;
        default:
            std::cout << "Unexpected button clicked." << std::endl;
            break;
    }
}

void NewInstanceDialog::remove_autoupdate_untouched()
{
    get_content_area()->remove(autoupdate);
    get_content_area()->remove(untouched);
}

void NewInstanceDialog::add_autoupdate_untouched(bool autoupdate_possible)
{
    if(autoupdate_possible)
    {
        get_content_area()->pack_start(autoupdate);
    }
    get_content_area()->pack_start(untouched);
}

void NewInstanceDialog::on_combo_changed()
{
    if(instance_type_combo.get_active_row_number() == 0)
    {
        get_content_area()->remove(buttons_box);
        get_content_area()->pack_start(version_entry);
        get_content_area()->pack_start(buttons_box);
        
        remove_autoupdate_untouched();

        get_content_area()->remove(browse_button);
        get_content_area()->remove(entry_and_button);

        add_autoupdate_untouched(false);
        version_entry.set_placeholder_text("Version");
        resize(310 - 52, 1); //Resizes to the minimum size possible. Works as a workaround for the dialog not resizing properly.
        show_all();
    }
    else if(instance_type_combo.get_active_row_number() == 1)
    {
        remove_autoupdate_untouched();
        get_content_area()->remove(version_entry);
        get_content_area()->remove(browse_button);
        get_content_area()->remove(entry_and_button);
        add_autoupdate_untouched(true);
        resize(310 - 52, 1);
        show_all();
    }
    else
    {
        remove_autoupdate_untouched();
        get_content_area()->remove(buttons_box);
        get_content_area()->remove(version_entry);
        get_content_area()->remove(browse_button);
        get_content_area()->pack_start(entry_and_button);
        get_content_area()->pack_start(version_entry);
        
        if(instance_type_combo.get_active_row_number() == 2)
        {
            get_content_area()->pack_start(browse_button);
            version_entry.set_placeholder_text("Path to the executable");
        }
        else
        {
            version_entry.set_placeholder_text("PR number");
        }
        add_autoupdate_untouched(false);
        resize(310 - 52, 1);
        show_all();
    }
}