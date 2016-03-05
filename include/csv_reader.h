#ifndef CSV_READER_H_INCLUDED
#define CSV_READER_H_INCLUDED
#include <string>
#include <fstream>
#include <sstream>
#include <util.h>
class CSV_reader
{
    public:
        CSV_reader() {}
        bool open(std::string filename);
        bool has_more();
        std::vector<std::string> get_row();    // Usage: while (csvr.has_more()) get_line...
    private:
//        std::string trim(std::string s, char c);
        std::ifstream fin;
        std::string line;
};

inline bool CSV_reader::open(std::string filename)
{
    fin.open(filename);
    return fin.is_open();
}

inline bool CSV_reader::has_more()
{
    bool b = std::getline(fin,line);
    if (!b)
        fin.close();
    return b;
}

inline std::vector<std::string> CSV_reader::get_row()
{
    std::vector<std::string> row;
    std::string d;
    std::stringstream lin(line);
    while (getline(lin, d, '"'))
    {
      if (d.size() > 1)
      {
  //      cout << "Part " << trim(d,',') << '\n';
        row.push_back(Util::trim(d,','));
      }
    }
    return row;
}
/*
inline std::string CSV_reader::trim(std::string s, char c)
{
  size_t p1 = s.find_first_not_of(c);
  size_t p2 = s.find_last_not_of(c);
//  cout << "trim: " << s << " " << c << '\n';
//  cout << "trim: " << p1 << " " << p2 << " " << p2-p1 << '\n';
  if (p1 ==  std::string::npos)
    p1=0;
  if (p2 == std::string::npos)
    p2=s.size();
//  cout << "trim: " << p1 << " " << p2 << " " << p2-p1 << '\n';
  return s.substr(p1,p2+1-p1);
}
*/
#endif // CSV_READER_H_INCLUDED
