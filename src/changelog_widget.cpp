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
    bool connected = Downloader::ping();
    if(connected)
    {
        const std::string changelog_url = "https://raw.githubusercontent.com/endless-sky/endless-sky/master/changelog";
        Downloader::download_buffered(changelog_url, changelog, false);
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