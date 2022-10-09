#pragma once
#include "gtkmm/checkbutton.h"
#include "gtkmm/hvbox.h"
#include <gtkmm.h>
#include <iostream>
//When creating an instance, this dialog will appear.
//It will ask the user to:
    //1. Introduce the type of instance (For now: Stable, Continuous and Custom)
    //2. Introduce the name of the instance
    //3. If the user chooses "Custom", it will ask for the executable of the instance
    //4. If the user chooses "Stable", it will ask for the version of the instance
    //5. If the user chooses "Continuous", won't ask for anything
//Then it will create a new instance with the given information
//The instance is another class, found in the "instance.hpp" file
class NewInstanceDialog : public Gtk::Dialog
{
  public:
    NewInstanceDialog()
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
        instance_type_combo.set_active(0);

        autoupdate.set_label("Autoupdate");

        get_content_area()->pack_start(untouched);
        untouched.set_label("Vanilla (No plugins)");
            
        buttons_box.pack_start(cancel_button);
        
        cancel_button.signal_clicked().connect(sigc::mem_fun(*this, &NewInstanceDialog::on_cancel_button_clicked));
        instance_type_combo.signal_changed().connect(sigc::mem_fun(*this, &NewInstanceDialog::on_combo_changed));
        browse_button.signal_clicked().connect(sigc::mem_fun(*this, &NewInstanceDialog::on_browse_button_clicked));
        show_all();      
    }
    bool auto_update()
    {
        return autoupdate.get_active();
    }
    bool vanilla()
    {
        return untouched.get_active();
    }
    int get_selected()
    {
        return instance_type_combo.get_active_row_number();
    }
    Gtk::HBox * get_buttons_box()
    {
        return &buttons_box;
    }
    //Returns the name of the instance
    //Note the naem(), since get_name() is already used by Gtk::Dialog
    std::string get_naem()
    {
        return name_entry.get_text();
    }
    //Returns the version of the instance. Unless it's continuous, which will return "0"
    std::string get_version()
    {
        return get_selected() != 1? version_entry.get_text() : "0";
    }
    //This returns the type of the instance. Please note that it has typee() instead of type(), since the get_type()
    //function is already defined in the Gtk::Dialog class (And returns a bunch of weird numbers)
    std::string get_typee()
    {
        return get_selected() == 0?"Stable":get_selected() == 1?"Continuous":"Custom";
    }
    
  private:
    Gtk::Button cancel_button{"Cancel"};
    Gtk::HeaderBar header;
    Gtk::Entry version_entry;
    Gtk::ComboBoxText instance_type_combo;
    Gtk::HBox buttons_box;
    Gtk::CheckButton autoupdate, untouched;
    //Exclusive for Custom instances
    Gtk::Button browse_button;
    Gtk::HBox entry_and_button;
    
    Gtk::Entry name_entry;
    //Callback for the cancel button
    void on_cancel_button_clicked()
    {
        close();
    }
    //Callback for the browse button (In case of custom instances)
    void on_browse_button_clicked()
    {
        Gtk::FileChooserDialog dialog("Please select the custom endless sky executable", Gtk::FILE_CHOOSER_ACTION_OPEN);
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
    void remove_autoupdate_untouched()
    {
        get_content_area()->remove(autoupdate);
        get_content_area()->remove(untouched);
    }
    void add_autoupdate_untouched(bool autoupdate_possible)
    {
        if(autoupdate_possible)
        {
            get_content_area()->pack_start(autoupdate);
        }
        get_content_area()->pack_start(untouched);
    }
    
    void on_combo_changed()
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
        else if(instance_type_combo.get_active_row_number() == 2)
        {
            remove_autoupdate_untouched();
            get_content_area()->remove(buttons_box);
            get_content_area()->remove(version_entry);
            get_content_area()->pack_start(entry_and_button);
            get_content_area()->pack_start(version_entry);
            get_content_area()->pack_start(browse_button);
            version_entry.set_placeholder_text("Path to the executable");
            add_autoupdate_untouched(false);
            resize(310 - 52, 1);
            show_all();
        }
    }
    
};
