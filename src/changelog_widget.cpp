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
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();
    FILE *fp;
    if (curl) 
    {
        fp = fopen((files_dir + "changelog.txt").c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, "https://raw.githubusercontent.com/endless-sky/endless-sky/master/changelog");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        //The write_data function will write the downloaded data to a file
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        //curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, xferinfo);
        /* pass the struct pointer into the xferinfo function */
        //Note: xferinfo is used to update the progress bar
        //curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

        if(res != CURLE_OK)
        {
            fprintf(stderr, "%s\n", curl_easy_strerror(res));
        }

        res = curl_easy_perform(curl);
        /* always cleanup */
        curl_easy_cleanup(curl);
        fclose(fp);
    }
    std::ifstream changelog_file("download/changelog.txt");
    std::string line;
    while (std::getline(changelog_file, line))
    {
        changelog += line + "\n";
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