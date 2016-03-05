#ifndef ACCOUNT_CHART_H_INCLUDED
#define ACCOUNT_CHART_H_INCLUDED

/// Kontoplan

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include <util.h>

class Account;

class Account_chart
{
  public:
    Account_chart() : number(0)
    {}
    Account_chart(Account_chart& a)
    {
      name = a.name;
      number = a.number;
      description = a.description;
      accounts = a.accounts;
    }

    template <class Action>
    void persist(Action &a)
    {
      Wt::Dbo::field(a, number, "number");
      Wt::Dbo::field(a, name, "name");
      Wt::Dbo::field(a, description, "description");
      Wt::Dbo::hasMany(a, accounts, Wt::Dbo::ManyToOne, "account_chart");
    }

    Wt::Dbo::collection<Wt::Dbo::ptr<Account> > accounts; // 'Fysiska' konton som tillhör den här kontoplansposten

  public:
    std::string get_num_name() const;

  private:
    short number; // Nummer från kontoplan
    std::string name;   // Namn, typ Lönekontot
    std::string description; // Beskrivning av kontot (Mat på ica, bilutgifter, bensin, godis....)
};

inline std::string Account_chart::get_num_name() const
{
  std::stringstream ss;
  ss << std::setw(4) << number << " - " << name;
  return ss.str();
}


#endif // ACCOUNT_CHART_H_INCLUDED
