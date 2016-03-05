#include "winman.h"
#include <ncurses.h>

#include <vector>
#include <algorithm>
#include <string>

#include <account_transaction.h>
#include <account.h>
#include <tag.h>
#include <user.h>

#define PUTTY_BACKSPACE 127
#define PUTTY_ESC 27
#define PUTTY_aa  165
#define PUTTY_AA  133
#define PUTTY_ae  164
#define PUTTY_AE  132
#define PUTTY_oe  182
#define PUTTY_OE  150

Winman::Winman() :
  menu_top_x(0),
  menu_top_y(4)
{
    initscr();
    start_color();
    use_default_colors();
    curs_set(0);
    noecho();
    raw();
    nonl();
    halfdelay(10);
    std::locale loc("sv_SE.UTF-8");
}

Winman::~Winman()
{
    endwin();
}

Winman::Winman(const Winman& other)
{
    //copy ctor
}

Winman& Winman::operator=(const Winman& rhs)
{
    if (this == &rhs) return *this; // handle self assignment
    //assignment operator
    return *this;
}

void Winman::update()
{
  size_t len=0;
  for (auto it=menu_items.begin(); it!=menu_items.end(); ++it)
  {
    if (it->second.length() > len)
      len=it->second.length();
  }
  menu_top_x = (COLS-(len+4))/2;
  int y = menu_top_y;
  for (auto it=menu_items.begin(); it!=menu_items.end(); ++it)
  {
    mvprintw(y, menu_top_x, "%c) %s", it->first, it->second.c_str());
    y+=2;
  }
  refresh();
}

void Winman::setup_db()
{
  try
  {
    pdb = new Wt::Dbo::backend::Postgres("host=127.0.0.1 user=bidgoat password=etthemligtpassord port=5432 dbname=bidgoat");
    //pdb.connect("host=127.0.0.1 port=5432 user=bidgoat pass=etthemligtpassord dbname=bidgoat");
  }
  catch (Wt::Dbo::Exception& e)
  {
    std::cerr << "Sket sig " << e.code();
    exit(EXIT_FAILURE);
  }
  session.setConnection(*pdb);
  session.mapClass<Account_transaction>("account_transaction");
  session.mapClass<Account>("account");
  session.mapClass<Tag>("tag");
  session.mapClass<User>("users");
  try
  {
    session.createTables();
  }
  catch (Wt::Dbo::Exception& e)
  {
    std::cerr << "Tables exist " << e.code() << '\n';
  }

}

void Winman::add_account()
{
  Wt::Dbo::Transaction  trans(session);
  Account* tmp_acc = new Account();
  int status = 0;
  auto vals = get_field_dialog(tmp_acc->get_field_names(), status, "Lägg till konto");
  if (status == 0)
  {
    if (tmp_acc->set_fields(vals))
    {
      Wt::Dbo::ptr<Account> tmp_acc_p = session.add(tmp_acc);
      trans.commit();
    }
  }
}

void Winman::add_user()
{
  Wt::Dbo::Transaction  trans(session);
  User* tmp = new User();
  int status = 0;
  auto vals = get_field_dialog(tmp->get_field_names(), status, "Lägg till användare");
  if (status == 0)
  {
    if (tmp->set_fields(vals))
    {
      Wt::Dbo::ptr<User> tmp_p = session.add(tmp);
      trans.commit();
    }
  }
}

bool Winman::isswedish(int ch)
{
  bool b = (ch == PUTTY_aa) ||
           (ch == PUTTY_AA) ||
           (ch == PUTTY_ae) ||
           (ch == PUTTY_AE) ||
           (ch == PUTTY_oe) ||
           (ch == PUTTY_OE);
  return b;
}

std::string Winman::utf8swed(int ch)
{
  switch(ch)
  {
    case PUTTY_aa:  return std::string("å"); break;
    case PUTTY_AA:  return std::string("Å"); break;
    case PUTTY_ae:  return std::string("ä"); break;
    case PUTTY_AE:  return std::string("Ä"); break;
    case PUTTY_oe:  return std::string("ö"); break;
    case PUTTY_OE:  return std::string("Ö"); break;
  }
  return "";
}

std::vector<std::string> Winman::get_field_dialog(std::vector<std::string> fields, int& status, std::string title)
{
  int h = fields.size()*3+4;
  int w = 50+4;
  WINDOW* form = newwin(h,w,3,(COLS-w)/2);
  mvwhline(form,0,1,'=',w-2);
  mvwvline(form,1,0,'|',h-2);
  mvwhline(form,h-1,1,'=',w-2);
  mvwvline(form,1,w-1,'|',h-2);
  mvwprintw(form,0,(w-title.length()-2)/2,"[%s]", title.c_str());
  int hpos=2;
  int vpos=2;
  for (auto f=fields.begin();f!=fields.end();++f )
  {
    mvwprintw(form, hpos, vpos,(*f).c_str());
    mvwchgat(form,hpos+1,vpos,50,A_REVERSE,0,NULL);
    hpos+=3;
  }

  hpos=3;
  wmove(form, hpos,vpos);
  curs_set(2);
  bool complete=false;
  int ch=0;
  std::string str;
  std::vector<std::string>  values(fields.size());
  size_t field=0;
  while (!complete)
  {
    wrefresh(form);
    ch=getch();
    switch (ch)
    {

      case 9: {
            values.at(field) = str;
            field++;
            if (field==fields.size()) // We're done
            {
              complete = true;
            }
            else
            {
              str.clear();
              hpos+=3;
              vpos=2;
              wmove(form, hpos,vpos);
            }
      } break;
      case 10:
      case 13: {
            values.at(field) = str;
//            values.push_back(str);
            complete=true;
            } break;
      case PUTTY_BACKSPACE: {
            vpos--;
            mvwaddch(form, hpos,vpos,' ');
            wmove(form, hpos,vpos);
            mvwchgat(form,hpos,vpos,1,A_REVERSE,0,NULL);
            str.pop_back();
            wrefresh(form); } break;
      case PUTTY_ESC: {
            status = 27;
            complete = true;
            } break;
    }
//    printw("%d", ch);
    if (isalnum(ch) || ch == 32 || isswedish(ch))
    {
      if (isswedish(ch))
      {
        std::string s = utf8swed(ch);
        str.append(s);
        mvwprintw(form, hpos,vpos,"%s",s.c_str());
        vpos++;
      }
      else
      {
        str.push_back(ch);
        mvwaddch(form,hpos,vpos,ch);
      }
      vpos++;
      wmove(form,hpos,vpos);
    }
  }
  werase(form);
  wrefresh(form);
  delwin(form);
  curs_set(0);

//  for (auto data=values.begin(); data!=values.end(); ++data)
//    std::transform(data->begin(), data->end(), data->begin(), ::tolower);

  return values;
}

void Winman::run()
{
  setup_db();
    mvprintw(0,(COLS-15)/2, "Account manager");
    mvchgat(0,0,-1,A_REVERSE,0,NULL);

    menu_items = { std::pair<char,std::string>('k',"Lägg till konto"), \
                   std::pair<char,std::string>('u',"Lägg till användare"), \
                   std::pair<char,std::string>('q',"Avsluta") };

    int ch=0;
    bool dorun=true;
    while (dorun)
    {
        update();
        ch=getch();
        //mvprintw(20,2,"%d",(int)ch);
        switch (ch)
        {
            case 'q':
            case 'Q': dorun=false; break;
            case 'k': add_account(); break;
            case 'u': add_user(); break;
        }
    }
}
