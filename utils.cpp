#ifndef UTILS_CPP
#define UTILS_CPP

#include "repo/transaction.h"

#include <iostream>
#include <chrono>
#include <functional>
#include <fstream>
#include <sstream>

using namespace std;
using namespace chrono;

inline void clearScreen()
{
#if defined(__unix__) || defined(__APPLE__)
   cout << "\033[2J\033[1;1H";
#elif defined(_WIN32)
   system("cls");
#else
   for (int i = 0; i < 50; ++i)
      cout << endl;
#endif
}

inline void printHeader(const string &title)
{
   cout << "========================================\n";
   cout << "     " << title << "\n";
   cout << "========================================\n\n";
}

inline string statusToString(OrderStatus status)
{
   switch (status)
   {
   case PAID:
      return "Paid";
   case COMPLETED:
      return "Completed";
   case CANCELED:
      return "Canceled";
   default:
      return "Unknown";
   }
}

inline bool isToday(const system_clock::time_point &timepoint)
{
   auto now = system_clock::now();
   time_t time_now = system_clock::to_time_t(now);
   time_t time_record = system_clock::to_time_t(timepoint);

   tm tm_now{};
   tm tm_record{};

#ifdef _WIN32
   localtime_s(&tm_now, &time_now);
   localtime_s(&tm_record, &time_record);
#else
   localtime_r(&time_now, &tm_now);
   localtime_r(&time_record, &tm_record);
#endif

   return (tm_now.tm_year == tm_record.tm_year &&
           tm_now.tm_mon == tm_record.tm_mon &&
           tm_now.tm_mday == tm_record.tm_mday);
}

inline pair<int, int> getMonthYear(const system_clock::time_point &timepoint)
{
   time_t time = system_clock::to_time_t(timepoint);
   tm *localTime = localtime(&time);
   return {localTime->tm_mon + 1, localTime->tm_year + 1900};
}

inline string maskBankId(uint id)
{
   string idStr = to_string(id);
   if (idStr.length() > 2)
   {
      return idStr.substr(0, 2) + string(idStr.length() - 2, 'x');
   }
   return idStr;
}

inline bool loadDataFromFile(const string &filename, function<bool(const string &)> processLine)
{
   ifstream file(filename);
   if (!file.is_open())
   {
      cout << "Data file not found: " << filename << ". Starting fresh for this data type." << endl;
      return false;
   }

   string line;
   int lineNumber = 0;
   bool success = true;

   while (getline(file, line))
   {
      lineNumber++;
      if (line.empty())
      {
         continue;
      }
      if (!processLine(line))
      {
         cerr << "Error processing line " << lineNumber << " in file " << filename << ": " << line << endl;
         success = false;

         break;
      }
   }

   file.close();
   return success;
}

template <typename Container, typename T = typename Container::value_type>
inline bool saveDataToFile(const string &filename, const Container &data_source, function<string(const T &)> formatItem)
{
   ofstream file(filename);
   if (!file.is_open())
   {
      cerr << "Error: Could not open file " << filename << " for saving." << endl;
      return false;
   }

   for (const T &item : data_source)
   {
      string line = formatItem(item);
      file << line << endl;
   }

   file.close();
   return true;
}

inline string replaceSpaces(string s, char replace_with = '_')
{
   replace(s.begin(), s.end(), ' ', replace_with);
   return s;
}

inline string restoreSpaces(string s, char replace_char = '_')
{
   replace(s.begin(), s.end(), replace_char, ' ');
   return s;
}

#endif