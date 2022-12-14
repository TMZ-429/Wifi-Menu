# include <boost/asio.hpp>
# include <bits/stdc++.h>
# include <json/json.h>
# include <ncurses.h>
# include <iostream>
# include <stdlib.h>
# include <iwlib.h>
# include <fstream>
# include <cstring>
# include <string>
# include <menu.h>
# include <chrono>
# include <thread>

# define CONFIG "/usr/local/bin/wifi-menu.d/wifi-menu-config.json"
# define SERVER_FILE "/usr/local/bin/wifi-menu.d/server > /dev/null &"
//This string when used with the system() function will initiate the server file and put the process to the side by allocating a thread to it.
# define SSID_LENGTH 30
//Lengths that all SSID's will be formatted to so the stats look uniform in the menu

using namespace boost::asio;
using namespace std;
using namespace ip;

string sendMessage(tcp::socket & socket, string msg);
string getTarget_SSID(string wirelessDevice);
void exitWindow(int errorcode);
void getPassword(string SSID, int port);
WINDOW * makeMenu(int iv);

struct WifiSignal {
    string SSID;
    double frequency;
    int quality;
};

int main(const int argc, char * argv[]) {
    Json::Value config;
    ifstream configuration(CONFIG, ifstream::binary);
    configuration >> config;
    cout << "Initiating server...\n";
    system(SERVER_FILE);
    std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    string wirelessDevice = config.get("wirelessDevice", "wlan0").asString(),
     SSID = (argv[1] ? argv[1] : getTarget_SSID(wirelessDevice)),
     //Either get the SSID from the terminal input or initiate the TUI menu to get it
     port_S = config.get("serverPort", "6891").asString(),
     typeArg = argv[2];
        int port = stoi(port_S);
        if (argc >= 2) {
            /*This will get the password from terminal input, attempt to connect to the signal then close the program.
            If there is no terminal input, then this will not run and the function for getting the password from the TUI menu will initiate.
            */
            boost::asio::io_service io_service;
            tcp::socket socket(io_service);
            cout << "Connecting to 127.0.0.1:" << port << "\n";
            socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port));
            string msg = "{\"ssid\": \"" + SSID + "\",\"password\":" + "\"" + argv[2] + "\"}",
            response = sendMessage(socket, msg),
            res = (response == "true" ? "Successfully connected to network '" + SSID + "'\n" : "Could not connect to network '" + SSID + "'\n");
            cout << res;
          return 0;
        }
       getPassword(SSID, port);
    return 0;
}

void exitWindow(int errorcode) {
    refresh();
    endwin();
    exit(errorcode);
} WINDOW * makeMenu(int iv) {
    int xMax, yMax;
      clear();
      initscr();
      noecho();
      cbreak();
      getmaxyx(stdscr, yMax, xMax);
     WINDOW * menu = newwin(iv, xMax - 12, yMax / 4 + 3, 6);
     //These mathematical equations will cause whatever menu is made to be formed in the centre of the screen no matter the dimensions.
    box(menu, 0, 0);
   return menu;
}
//I found myself copying and pasting the same code over and over to make menus and exit the process, so I simplified it by making these functions.

string getTarget_SSID(string wirelessDevice) {
    int yMax, xMax, wifiScan_I;
    char * device_W = strcpy(new char[wirelessDevice.length() + 1], wirelessDevice.c_str());
    struct WifiSignal Signals[100];
    string device_T;
    wireless_scan_head head;
    wireless_scan * result;
    iwrange range;
    int socket = iw_sockets_open();
      if (iw_get_range_info(socket, device_W, &range)) {
          printw("[ERROR]: cannot get range using %s\n", device_W);
          exitWindow(1);
      } if (iw_scan(socket, device_W, range.we_version_compiled, &head)) {
          printw("[ERROR]: cannot scan for signals\n");
          exitWindow(2);
      };
        result = head.result;
        for (wifiScan_I = 0; result; wifiScan_I++) {
            Signals[wifiScan_I].SSID = result -> b.essid;
            Signals[wifiScan_I].frequency = result -> b.freq;
            Signals[wifiScan_I].quality = result -> stats.qual.qual;
            result = result -> next;
            //This will iterate through the scan of local wifi signals while putting the useful information into an array.
        }
        WINDOW * menu = makeMenu(wifiScan_I + 2);
        //We keep the wifiScan_I variable so we can use it to determine the height we need to make the box showing each signal and so we can easily determine the length of the array.
        getmaxyx(stdscr, yMax, xMax);
        choose_O: {
          box(menu, 0, 0);
          printw("Use the arrow keys to navigate the menu and press enter to select your option.\n\n");
          refresh();
          keypad(menu, true);
             int choice, highlight = 0;
             WINDOW * lwin = newwin(3, xMax - 12, yMax / 4, 6);
             box(lwin, 0, 0);
             refresh();
             mvwprintw(lwin, 1, 1, "|             SSID            |   GHZ    | Quality |");
             wrefresh(lwin);
             //I don't use the makeMenu function here because I want the box to be specifically above the centre
            while (true) {
                for (int i = 0; i < wifiScan_I; i++) {
                    //This will make a menu then loop through everything in the Signals array and add it to the menu
                    if (i == highlight) { wattron(menu, A_REVERSE); };
                    string SSID = Signals[i].SSID, frequency = ("| " + to_string(Signals[i].frequency / 1000000000) + " | "), quality = (to_string(Signals[i].quality) + " | ");
                    while (SSID.length() < SSID_LENGTH) { SSID += " "; };
                    /*This while loop will format the lines so they look more like this in the menu:
                         ┌──────────────────────────────────────────────────────────────────┐
                         │|             SSID            |   GHZ    | Quality |              │
                         └──────────────────────────────────────────────────────────────────┘
                         ┌──────────────────────────────────────────────────────────────────┐
                         │IHDSB.CA                      | 2.412000 | 70 |                   │
                         │MYHDSB                        | 2.412000 | 32 |                   │
                         │MYHDSB.CA                     | 2.412000 | 70 |                   │

                    As opposed to this if I didn't format them which would look more like:
                         ┌──────────────────────────────────────────────────────────────────┐
                         │|             SSID            |   GHZ    | Quality |              │
                         └──────────────────────────────────────────────────────────────────┘
                         ┌──────────────────────────────────────────────────────────────────┐
                         │IHDSB.CA | 2.412000 | 70 |                   │
                         │MYHDSB | 2.412000 | 32 |                   │
                         │MYHDSB.CA | 2.412000 | 70 |                   │
                         │Pizzahut public wifi | 2.412000 | 81 |                   │
                        (It would look very disorganized if I didn't make every SSID a uniform length with blank spaces.).
                    */
                    string signalInfo[4] = {SSID, frequency, quality};
                    int length = 1;
                    for (string info: signalInfo) {
                        mvwprintw(menu, i + 1, length, info.c_str());
                        length += info.length();
                    }
                    wattroff(menu, A_REVERSE);
                }
                choice = wgetch(menu);
                //Ordinarily, you'd just use a switch case to make menu navigation, however, I wanted to make <- and -> also iterate through the menu.
                if (choice == KEY_UP || choice == KEY_LEFT) {
                    highlight--;
                    //This makes sure the "cursor" doesn't go beyond the lower limits of the menu
                    if (highlight <= -1) { highlight = wifiScan_I - 1; };
                } else if (choice == KEY_DOWN || choice == KEY_RIGHT) {
                    highlight++;
                    if (highlight >= wifiScan_I) { highlight = 0; };
                } else if (choice == 10) { break; };
            }
            printw("You picked '%s'.\nPress enter to continue or esc to go back.", Signals[highlight].SSID.c_str());
            refresh();
           int keypress = wgetch(menu);
           if (keypress == 10) {
               printw("Exiting window...\n");
               endwin();
               return Signals[highlight].SSID;
           } else {
               printw("\nAborted\n");
               clear();
               goto choose_O;
           }
        }
}

void getPassword(string SSID, int port) {
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    printw("\nConnecting to 127.0.0.1:" + port);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), port));
    string buffer;
    WINDOW * prompt = makeMenu(3);
    string msg = "Please enter the password for: ";
    msg += SSID;
    printw(msg.c_str());
    refresh();
    wrefresh(prompt);
    keypad(prompt, true);
    char input;
    int input_I;
    while (true) {
        input = wgetch(prompt);
        input_I = input;
        if (input == 10) {
            break;
        } else if (input_I == 7 && buffer.length() > 0) {
            buffer = buffer.substr(0, buffer.size() - 1);
        } else if (input_I != 7) {
            string character(1, input);
            buffer += character;
        }
        mvwprintw(prompt, 1, 1, buffer.c_str());
        wrefresh(prompt);
        /*This function is fairly straight-forward -- the program makes a box for input and every character of input is added to a variable that's printed to said box.
        When enter is pressed, the loop ends and the variable that makes up the wifi password is finalized
        */
    }
    endwin();
    //There's no real way to actually connect to wifi in C++ (At least not one I could find) so I instead send the information through a server to a NodeJS program that uses it to connect to the wifi.
    msg = "{\"ssid\": \"" + SSID + "\",\"password\":" + "\"" + buffer + "\"}";
    string response = sendMessage(socket, msg);
    if (response == "true") {
        cout << "Successfully connected to network '" << SSID << "'\n";
    } else {
        cout << "Connection to network '" << SSID << "' failed\n";
        exit(3);
    }
}

string sendMessage(tcp::socket & socket, string msg) {
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);
    boost::asio::streambuf receive_buffer;
    boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
    string response = boost::asio::buffer_cast<const char*>(receive_buffer.data());
    return response;
}