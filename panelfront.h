#ifndef __PANELFRONT_H__
#define __PANELFRONT_H__

#include "config.h"

#if (defined(HAVE_LIBNCURSES) && defined(HAVE_LIBPANEL) && defined(BUILD_PANEL))

#ifdef HAVE_LIBCURSES
#include <curses.h>
#include <panel.h>
#else
#include <ncurses.h>
#include <panel.h>
#endif
#undef erase
#undef clear
#undef move

#include <stdio.h>
#include <time.h>
#include <list>
#include <map>
#include <string>

#include "packet.h"
#include "frontend.h"
#include "cursesfront.h"
#include "networksort.h"

#define COLOR_DEFAULT -1

// These are in the kismet_curses.cc main code
extern int sound;
extern int speech;
extern unsigned int metric;

class PanelFront : public Frontend {
public:
    PanelFront();

    void AddPrefs(map<string, string> in_prefs);

    int ParseArgs(int argc, char *argv[]) { return 0; }

    int Tick();

    int Poll();

    int InitDisplay(int in_decay, time_t in_start);

    // Draw the screen
    int DrawDisplay();

    // End
    int EndDisplay();

    int WriteStatus(string status);

protected:
    // Curses color pair
    typedef struct color_pair {
        color_pair() { index = -1; pair = 0; bold = 0; }

        int index;
        int bold;
        int pair;

    };

    // Tracking for our subwindows so we can spawn more than one at once.
    // Having to cast this all the time is annoying, but...
    typedef int (PanelFront::*panel_printer)(void *);
    typedef int (PanelFront::*key_handler)(void *, int);

    // This can be filled in by the generic panel spawner or by a special
    // spawner, like help is.
    typedef struct {
        // Window and panel
        WINDOW *win;
        PANEL *pan;
        // Function pointer to the function that handles our printing
        panel_printer printer;
        // Keyboard event handler
        key_handler input;
        // Title of window
        string title;

        // Not everything will use all of these but they're available
        // Start of sliding window over the data
        int start;
        // End of sliding window over the data
        int end;
        // Selected item
        int selected;
        // Printable lines for scrolling window
        int max_display;
        // Width of printable area
        int print_width;
        // Paused
        int paused;

        // Text, if we store it seperately
        vector<string> text;
    } kis_window;

    // All the windows
    list<kis_window *> window_list;

    // Printers for our main 3 panels... This pulls MOST of this out of class globals,
    // but not quite all.
    int MainNetworkPrinter(void *in_window);
    int MainInfoPrinter(void *in_window);
    int MainStatusPrinter(void *in_window);

    // Just print the stored text
    int TextPrinter(void *in_window);
    // Various popups that generate stuff dynamically
    int SortPrinter(void *in_window);
    int PowerPrinter(void *in_window);
    int DetailsPrinter(void *in_window);
    int DumpPrinter(void *in_window);
    int GroupNamePrinter(void *in_window);
    int RatePrinter(void *in_window);
    int StatsPrinter(void *in_window);
    int PackPrinter(void *in_window);

    // Keyboard handlers
    int MainInput(void *in_window, int in_chr);
    int SortInput(void *in_window, int in_chr);
    int PowerInput(void *in_window, int in_chr);
    int DetailsInput(void *in_window, int in_chr);
    int DumpInput(void *in_window, int in_chr);
    // Group titler skips all of this
    int RateInput(void *in_window, int in_chr);
    int StatsInput(void *in_window, int in_chr);
    int PackInput(void *in_window, int in_chr);
    // Help has a generic handler
    int TextInput(void *in_window, int in_chr);

    // Spawn a generic popup
    void SpawnWindow(string in_title, panel_printer in_print, key_handler in_input,
                    int in_x = -1, int in_y = -1);

    // Spawn a help popup
    void SpawnHelp(char **in_helptext);

    // Kill a window
    void DestroyWindow(kis_window *in_window);

    void RescaleDisplay();

    void ZoomNetworks();

    void Details2Vector(wireless_network *in_net);

    string NetLine(wireless_network *net, const char *name, int sub, int group, int expanded, int tagged);

    void SetColumns(string in_columns);

    void MuteToggle();

    color_pair ColorParse(string in_color);

    string Mac2String(uint8_t *mac, char seperator);

    int color;

    int clear_dump;

    sort_type sortby;

    vector<string> column_vec;

    int quality, power, noise;

    int zoomed;

    display_network *details_network;
    vector<display_network *> last_displayed;

    int hsize, vsize;

    // Map of bssid's to display groups
    map<string, display_network *> bssidgroup_map;

    // Map of BSSID's to custom names
    map<string, string> bssidname_map;

    int old_sound;
    int old_speech;
    int muted;

    vector<int> packet_history;

    // Size of the group vec the last time we drew it
    int last_draw_size;

    // Battery monitoring states
    unsigned int monitor_bat;
    unsigned int bat_percentage;
    unsigned int bat_time;
    unsigned int bat_available;
    unsigned int bat_ac;
    unsigned int bat_charging;

    // Statistics
    int max_packet_rate;

    // Keep these three here so we can refer to them easily - they're non-transient
    kis_window *net_win;
    kis_window *info_win;
    kis_window *stat_win;

    // Current one
    kis_window *cur_window;

    map<string, color_pair> color_map;

};

#endif

#endif
