#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "terminal.h"
#include "suggestions.h"
#include "outline.h"
#include "syntax.h"
#include <fstream>

using namespace std;
using namespace ide;

int main(int argc, char** argv) {
    cout << "Displexity CLI IDE - Integrated Terminal, Suggestions, Outline\n";
    if (argc < 2) {
        cout << "Usage: ide\\cli.exe <file.dis>\n";
        return 1;
    }
    string file = argv[1];
    cout << "Opening file: " << file << "\n\n";

    // Show outline
    auto outline = get_outline_from_file(file);
    cout << "== Outline ==\n";
    if (outline.empty()) cout << "(no symbols found)\n";
    for (auto &s : outline) cout << s << "\n";
    cout << "\n";

    // Show highlighted preview (first 20 lines)
    cout << "== Preview (highlighted) ==\n";
    std::ifstream fin(file);
    string l;
    int cnt = 0;
    while (cnt < 20 && std::getline(fin, l)) {
        cout << ide::highlight_line(l, true) << "\n";
        cnt++;
    }
    cout << "\n";

    // REPL
    string line;
    while (true) {
        cout << ":> ";
        if (!getline(cin, line)) break;
        if (line == "exit" || line == "quit") break;
        if (line.rfind(":run ", 0) == 0) {
            string cmd = line.substr(5);
            cout << "[running] " << cmd << "\n";
            string out = run_command_capture(cmd);
            cout << out << "\n";
            continue;
        }
        if (line.rfind(":sug ", 0) == 0) {
            string prefix = line.substr(5);
            auto sug = get_suggestions_from_file(file, prefix);
            cout << "Suggestions for '" << prefix << "':\n";
            for (auto &s : sug) cout << "  " << s << "\n";
            continue;
        }
        if (line.rfind(":git ", 0) == 0) {
            string arg = line.substr(5);
            if (arg == "status") {
                string out = run_command_capture("git status --short");
                cout << out << "\n";
            } else if (arg == "branch") {
                string out = run_command_capture("git branch --show-current");
                cout << "Branch: " << out << "\n";
            } else if (arg == "log") {
                string out = run_command_capture("git --no-pager log --oneline -n 10");
                cout << out << "\n";
            } else {
                cout << "Git commands: status, branch, log\n";
            }
            continue;
        }
        if (line.rfind(":theme ", 0) == 0) {
            string t = line.substr(7);
            if (t == "dark") {
                cout << "Theme set to dark (terminal preview uses dark colors)\n";
            } else if (t == "light") {
                cout << "Theme set to light (terminal preview uses light colors)\n";
            } else {
                cout << "Usage: :theme dark|light\n";
            }
            continue;
        }
        if (line == ":outline") {
            auto o = get_outline_from_file(file);
            cout << "Outline:\n";
            for (auto &s : o) cout << "  " << s << "\n";
            continue;
        }
        if (line.rfind(":edit ", 0) == 0) {
            string cmd = string("notepad ") + file;
            cout << "Opening editor...\n";
            run_command_capture(cmd);
            continue;
        }
        cout << "Commands:\n";
        cout << "  :run <cmd>    - run shell command and show output\n";
        cout << "  :sug <prefix> - show token suggestions from file\n";
        cout << "  :outline      - show outline\n";
        cout << "  :edit <file>  - open file in external editor\n";
        cout << "  exit / quit   - exit\n";
    }

    cout << "Goodbye.\n";
    return 0;
}
