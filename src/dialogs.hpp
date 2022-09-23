#pragma once
#include <gtkmm.h>
#include <iostream>

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
        
        
        get_content_area()->pack_start(name_entry); 
        get_content_area()->pack_start(instance_type_combo);
        get_content_area()->pack_start(version_entry);
        version_entry.set_placeholder_text("Version");
        get_content_area()->pack_start(buttons_box);

        instance_type_combo.append("Stable release");
        instance_type_combo.append("Continuous build");
        instance_type_combo.append("Custom instance");
        instance_type_combo.set_active(0);
            
        buttons_box.pack_start(cancel_button);
        buttons_box.pack_start(ok_button);
        ok_button.grab_focus();
        
        ok_button.signal_clicked().connect(sigc::mem_fun(*this, &NewInstanceDialog::on_ok_button_clicked));
        cancel_button.signal_clicked().connect(sigc::mem_fun(*this, &NewInstanceDialog::on_cancel_button_clicked));
        instance_type_combo.signal_changed().connect(sigc::mem_fun(*this, &NewInstanceDialog::on_combo_changed));
        show_all();      
    }
    int get_selected()
    {
        return instance_type_combo.get_active_row_number();
    }
    
  private:
    Gtk::Button ok_button{"OK"}, cancel_button{"Cancel"};
    Gtk::HeaderBar header;
    Gtk::Entry version_entry;
    Gtk::ComboBoxText instance_type_combo;
    Gtk::HBox buttons_box;
    
    Gtk::Entry name_entry;
    void on_ok_button_clicked()
    {
        close();
    }
    void on_cancel_button_clicked()
    {
        close();
    }
    void on_combo_changed()
    {
        if(instance_type_combo.get_active_row_number() == 0)
        {
            get_content_area()->remove(buttons_box);
            get_content_area()->pack_start(version_entry);
            get_content_area()->pack_start(buttons_box);
            version_entry.set_placeholder_text("Version");
            resize(310 - 52, 1); //Resizes to the minimum size possible. Works as a workaround for the dialog not resizing properly.
            show_all();
        }
        else
        {
            get_content_area()->remove(version_entry);
            resize(310 - 52, 1);
            show_all();
        }
    }
    
};
inline void new_dialog()
{
    NewInstanceDialog dialog;
    dialog.run();
}
