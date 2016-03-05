#ifndef WINMAN_H
#define WINMAN_H

#include <vector>
#include <string>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>

class Winman
{
    public:
        Winman();
        virtual ~Winman();
        Winman(const Winman& other);
        Winman& operator=(const Winman& other);

        void run();
        void update();
        void add_account();
        void add_user();
//        void open_form(std::vector<std::string> fields);

    protected:
      void setup_db();
      bool isswedish(int ch);
      std::string utf8swed(int ch);
      std::vector<std::string> get_field_dialog(std::vector<std::string> fields, int& status, std::string title="");

      std::vector<std::pair<char, std::string>> menu_items;

      Wt::Dbo::backend::Postgres*  pdb;
      Wt::Dbo::Session  session;

    private:
      int menu_top_x;
      int menu_top_y;
};

#endif // WINMAN_H
