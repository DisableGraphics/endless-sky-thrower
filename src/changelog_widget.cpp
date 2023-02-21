#include "changelog_widget.hpp"
#include "downloader.hpp"

ChangelogWidget::ChangelogWidget()
{

    std::cout << "[INFO] Downloading changelog..." << std::endl;
    download_changelog();
    std::cout << "[INFO] Changelog downloaded." << std::endl;
    
    set_changelog();
}

void ChangelogWidget::download_changelog()
{
    const std::string changelog_url = "https://raw.githubusercontent.com/endless-sky/endless-sky/master/changelog";
    Downloader::download(changelog_url, (files_dir + "changelog.txt"), false);
    
    std::ifstream changelog_file("download/changelog.txt");
    if(changelog_file.good())
    {
        std::string line;
        while (std::getline(changelog_file, line))
        {
            changelog += line + "\n";
        }
    }
    else 
    {
        changelog = "Unable to download changelog. Please check your internet connection.";
    }

}

void ChangelogWidget::set_changelog()
{
    changelog_label.set_markup("<b>Changelog</b>");

    changelog_textview.set_editable(false);
    changelog_textview.set_cursor_visible(false);
    changelog_textview.set_wrap_mode(Gtk::WRAP_WORD);
    changelog_textview.get_buffer()->set_text(changelog);

    scrolled_window.add(changelog_textview);
    scrolled_window.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);
    scrolled_window.set_shadow_type(Gtk::SHADOW_ETCHED_IN);
    pack_start(changelog_label, Gtk::PACK_SHRINK);
    pack_start(separator, Gtk::PACK_SHRINK);
    pack_start(scrolled_window, Gtk::PACK_EXPAND_WIDGET);
    show_all_children();
}