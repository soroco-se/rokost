#ifndef ACCOUNT_H_INCLUDED
#define ACCOUNT_H_INCLUDED

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>
#include <util.h>

class Account_transaction;
class Account_chart;
class Tag;

class Account
{
  public:
    Account() : account_number(0)
    {}
    Account(Account& a)
    {
      type = a.type;
      name = a.name;
      number = a.number;
      account_number = a.account_number;
      desc_str = a.desc_str;
      dbt_transactions = a.dbt_transactions;
      cdt_transactions = a.cdt_transactions;
      tags = a.tags;
      account_group = a.account_group;
    }
    Account(std::string t, std::string n, std::string nb) : type(t), name(n), number(nb), account_number(0)
    {}
    Account(std::string desc)  : account_number(0), desc_str(desc)
    {
      set_fields_from_desc(desc);
    }

    double get_balance() const;
    std::vector<std::string> get_field_names();     // Returns the fields names
    bool set_fields(std::vector<std::string>& f_vals);  // In the order returned by get_field_names
    void set_fields_from_desc(std::string d);

    template <class Action>
    void persist(Action &a)
    {
      Wt::Dbo::field(a, type, "type");
      Wt::Dbo::field(a, name, "name");
      Wt::Dbo::field(a, number, "number");
      Wt::Dbo::field(a, account_number, "account_number");
      Wt::Dbo::field(a, desc_str, "desc_str");
      Wt::Dbo::hasMany(a, dbt_transactions, Wt::Dbo::ManyToOne, "debit_transactions"); // Debiteringar av kontot, minskar beloppet på kontot
      Wt::Dbo::hasMany(a, cdt_transactions, Wt::Dbo::ManyToOne, "credit_transactions"); // Kreditering av kontot, ökar beloppet på kontot
      Wt::Dbo::hasMany(a, tags, Wt::Dbo::ManyToMany, "account_transaction_tags"); // Added from web interface
      Wt::Dbo::belongsTo(a, account_group, "account_chart");   // Tillhör detta konto från kontoplanen
    }

    Wt::Dbo::collection<Wt::Dbo::ptr<Account_transaction> > dbt_transactions; // Transaktioner som berör kontot
    Wt::Dbo::collection<Wt::Dbo::ptr<Account_transaction> > cdt_transactions; // Transaktioner som berör kontot
    Wt::Dbo::collection<Wt::Dbo::ptr<Tag> > tags;
    Wt::Dbo::ptr<Account_chart> account_group;

  private:
    std::string type;   // Konto, Utgiftsområde osv...
    std::string name;   // Namn, typ Lönekontot
    std::string number; // Kontonummer, om "riktigt" konto
    short    account_number; // Kontonummer från kontoplan
    std::string desc_str; // Sträng så som den ser ut i description-fältet i kontoutdraget, utan förled. dsv
                          // om desc="Betalning PG 4875503-7 FORTUM MARKETS" så är desc_str="PG 4875503-7 FORTUM MARKETS"
                          // "Betalning" används som tag.
    // Lägg till relation till
};

inline double Account::get_balance() const
{
  double bal=0;
  for (auto it : dbt_transactions)
  {
    bal+=it->get_amount();
  }
  for (auto it : cdt_transactions)
  {
    bal+=it->get_amount();
  }
  return bal;
}

// Oh, divine lord please forgive me
inline std::vector<std::string> Account::get_field_names()
{
    return {"type", "name", "number"};
}

inline bool Account::set_fields(std::vector<std::string>& f_vals)
{
  if (f_vals.size() != 3)
    return false;
  type = f_vals.at(0);
  name = f_vals.at(1);
  number  = f_vals.at(2);
  return true;
}

inline void Account::set_fields_from_desc(std::string d)
{
  // Hardcoded strings... sorry
  // Om börjar på BG/PG/lån, parsa numret
  size_t p;
  if ( (p=d.find("BG ")) != std::string::npos && p<3)
    type = "BG";
  else if ( (p=d.find("PG ")) != std::string::npos && p<3)
    type = "PG";
  else if (d.find("lån ") != std::string::npos)
    type = "loan";
  size_t b = d.find_first_of("0123456789");
  size_t e = d.find_last_of("0123456789");
  if (b != std::string::npos && e != std::string::npos && (e-b)>7)  // Försök röka ut affärsställenummer (FEMMANS SPORT 31179)
  {
    number = Util::trim(d.substr(b, e-b+1),' ');
  }
  if (e != std::string::npos)
  {
    name = Util::trim(d.substr(e+1),' ');
  }



}

#endif // ACCOUNT_H_INCLUDED
