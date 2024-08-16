#pragma once

#include <format>
#include <string>
#include <iostream>
class Log {
    static inline const char *BLACK_FG           =   ";30";
    static inline const char *RED_FG             =   ";31";
    static inline const char *GREEN_FG           =   ";32";
    static inline const char *YELLOW_FG          =   ";33";
    static inline const char *BLUE_FG            =   ";34";
    static inline const char *MAGENTA_FG         =   ";35";
    static inline const char *CYAN_FG            =   ";36";
    static inline const char *WHITE_FG           =   ";37";

    static inline const char *BLACK_ON_BLACK     = "30;40";
    static inline const char *BLACK_ON_RED       = "30;41";
    static inline const char *BLACK_ON_GREEN     = "30;42";
    static inline const char *BLACK_ON_YELLOW    = "30;43";
    static inline const char *BLACK_ON_BLUE      = "30;44";
    static inline const char *BLACK_ON_MAGENTA   = "30;45";
    static inline const char *BLACK_ON_CYAN      = "30;46";
    static inline const char *BLACK_ON_WHITE     = "30;47";
    static inline const char *RED_ON_BLACK       = "31;40";
    static inline const char *RED_ON_RED         = "31;41";
    static inline const char *RED_ON_GREEN       = "31;42";
    static inline const char *RED_ON_YELLOW      = "31;43";
    static inline const char *RED_ON_BLUE        = "31;44";
    static inline const char *RED_ON_MAGENTA     = "31;45";
    static inline const char *RED_ON_CYAN        = "31;46";
    static inline const char *RED_ON_WHITE       = "31;47";
    static inline const char *GREEN_ON_BLACK     = "32;40";
    static inline const char *GREEN_ON_RED       = "32;41";
    static inline const char *GREEN_ON_GREEN     = "32;42";
    static inline const char *GREEN_ON_YELLOW    = "32;43";
    static inline const char *GREEN_ON_BLUE      = "32;44";
    static inline const char *GREEN_ON_MAGENTA   = "32;45";
    static inline const char *GREEN_ON_CYAN      = "32;46";
    static inline const char *GREEN_ON_WHITE     = "32;47";
    static inline const char *YELLOW_ON_BLACK    = "33;40";
    static inline const char *YELLOW_ON_RED      = "33;41";
    static inline const char *YELLOW_ON_GREEN    = "33;42";
    static inline const char *YELLOW_ON_YELLOW   = "33;43";
    static inline const char *YELLOW_ON_BLUE     = "33;44";
    static inline const char *YELLOW_ON_MAGENTA  = "33;45";
    static inline const char *YELLOW_ON_CYAN     = "33;46";
    static inline const char *YELLOW_ON_WHITE    = "33;47";
    static inline const char *BLUE_ON_BLACK      = "34;40";
    static inline const char *BLUE_ON_RED        = "34;41";
    static inline const char *BLUE_ON_GREEN      = "34;42";
    static inline const char *BLUE_ON_YELLOW     = "34;43";
    static inline const char *BLUE_ON_BLUE       = "34;44";
    static inline const char *BLUE_ON_MAGENTA    = "34;45";
    static inline const char *BLUE_ON_CYAN       = "34;46";
    static inline const char *BLUE_ON_WHITE      = "34;47";
    static inline const char *MAGENTA_ON_BLACK   = "35;40";
    static inline const char *MAGENTA_ON_RED     = "35;41";
    static inline const char *MAGENTA_ON_GREEN   = "35;42";
    static inline const char *MAGENTA_ON_YELLOW  = "35;43";
    static inline const char *MAGENTA_ON_BLUE    = "35;44";
    static inline const char *MAGENTA_ON_MAGENTA = "35;45";
    static inline const char *MAGENTA_ON_CYAN    = "35;46";
    static inline const char *MAGENTA_ON_WHITE   = "35;47";
    static inline const char *CYAN_ON_BLACK      = "36;40";
    static inline const char *CYAN_ON_RED        = "36;41";
    static inline const char *CYAN_ON_GREEN      = "36;42";
    static inline const char *CYAN_ON_YELLOW     = "36;43";
    static inline const char *CYAN_ON_BLUE       = "36;44";
    static inline const char *CYAN_ON_MAGENTA    = "36;45";
    static inline const char *CYAN_ON_CYAN       = "36;46";
    static inline const char *CYAN_ON_WHITE      = "36;47";
    static inline const char *WHITE_ON_BLACK     = "37;40";
    static inline const char *WHITE_ON_RED       = "37;41";
    static inline const char *WHITE_ON_GREEN     = "37;42";
    static inline const char *WHITE_ON_YELLOW    = "37;43";
    static inline const char *WHITE_ON_BLUE      = "37;44";
    static inline const char *WHITE_ON_MAGENTA   = "37;45";
    static inline const char *WHITE_ON_CYAN      = "37;46";
    static inline const char *WHITE_ON_WHITE     = "37;47";

    static inline const char *BOLD               =    ";1";
    static inline const char *FAINT              =    ";2";
    static inline const char *ITALICS            =    ";3";
    static inline const char *UNDERLINE          =    ";4";
    static inline const char *CROSSED_OUT        =    ";9";


public:
  template<class ...Args>
  static inline void Info(Args&& ...args) {
    pColorText((s(BLACK_ON_BLUE) + s(BOLD)+ s(UNDERLINE)).c_str(), "[INFO]");
    pColorText(BLUE_FG, ": ");
    pColorTextLn(BLUE_FG, std::forward<Args>(args)...);
  }

  template<class ...Args>
  static inline void Success(Args&& ...args) {
    pColorText((s(BLACK_ON_GREEN) + s(BOLD)+ s(UNDERLINE)).c_str(), "[SUCCESS]");
    pColorText(GREEN_FG, ": ");
    pColorTextLn(GREEN_FG, std::forward<Args>(args)...);
  }

  template<class ...Args>
  static inline void Warning(Args&& ...args) {
    pColorText((s(BLACK_ON_YELLOW) + s(BOLD)+ s(UNDERLINE)).c_str(), "[WARNING]");
    pColorText(YELLOW_FG, ": ");
    pColorTextLn(YELLOW_FG, std::forward<Args>(args)...);
  }

  template<class ...Args>
  static inline void Error(Args&& ...args) {
    pColorText((s(BLACK_ON_RED) + s(BOLD)+ s(UNDERLINE)).c_str(), "[ERROR]");
    pColorText(RED_FG, ": ");
    pColorTextLn(RED_FG, std::forward<Args>(args)...);
  }

private:
  template<class ...Args>
  static void pColorText(const char *color, Args&& ...args) {
    std::clog << "\033[" << color << "m";
    (std::clog << ... << args) << "\033[0m";//std::format("\033[{}m{}\033[0m", std::make_format_args(color, args...));
  }

  template<class ...Args>
  static void pColorTextLn(const char *color, Args&& ...args) {
    std::clog << "\033[" << color << "m";
    (std::clog << ... << args) << "\033[0m\n";//std::format("\033[{}m{}\033[0m", std::make_format_args(color, args...));
  }

  static std::string s(const char *c_str) {
    return std::string(c_str);
  }
};