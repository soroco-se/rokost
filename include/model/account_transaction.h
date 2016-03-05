#ifndef Account_transaction_H
#define Account_transaction_H

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

class Tag;
class Account;
class User;

class Account_transaction
{
  public:
    Account_transaction() {}
    Account_transaction(Account_transaction& a)
    {
      amount = a.amount;
      transaction_date = a.transaction_date;
      transaction_str = a.transaction_str;
      transaction_tag = a.transaction_tag;
      tags = a.tags;
      dbt = a.dbt;
      cdt = a.cdt;
    }

    bool set(std::vector<std::string>&& vs)
    {
      bool ret = true;
      if (vs.size() < 3)
        return false;
      set_date(vs[0]);
      transaction_str = vs[1];

      if (vs.size() == 3)
      {
        ret = set_amount(vs[2]);
      }
      else if (vs.size() == 4)
      {
        transaction_tag = vs[2];
        ret = set_amount(vs[3]);
      }
      return ret;
    }

    void set_date(std::string d)
    {
      transaction_date = Wt::WDate::fromString(d,"yyyy-MM-dd");
    }

    bool set_amount(std::string a)
    {
      try
      {
        amount = std::stod(a);
      }
      catch (std::exception e)
      {
        return false;
      }
      return true;
    }

    double get_amount() const { return amount; }

    bool is_debit()
    {
      return (amount < 0);
    }

    template <class Action>
    void persist(Action &a)
    {
      Wt::Dbo::field(a, transaction_date, "date");        // From transaction info file
      Wt::Dbo::field(a, transaction_str, "description");         // From transaction info file
      Wt::Dbo::field(a, transaction_tag, "tag");          // From transaction info file
      Wt::Dbo::field(a, amount, "amount");                // From transaction info file
      Wt::Dbo::belongsTo(a, dbt, "debit_transactions");   // Set if amount is negative, and the account affected is identifiable
      Wt::Dbo::belongsTo(a, cdt, "credit_transactions");  // Set if amount is positive, and the account affected is identifiable
      Wt::Dbo::hasMany(a, tags, Wt::Dbo::ManyToMany, "account_transaction_tags"); // Added from web interface
      Wt::Dbo::field(a, comment, "comment");                // Added from web interface
      Wt::Dbo::belongsTo(a, usr, "updated_transactions");              // Which user updated this entry
      Wt::Dbo::field(a, update_date, "updated");
    }

    friend std::ostream& operator<<(std::ostream& os, Account_transaction a)
    {
      os << a.transaction_date.toString() << " '" << a.transaction_str << "' '" << a.transaction_tag << "' " << a.amount;
      return os;
    }


    Wt::Dbo::collection<Wt::Dbo::ptr<Tag> > tags;
    Wt::Dbo::ptr<Account> dbt;
    Wt::Dbo::ptr<Account> cdt;
    Wt::Dbo::ptr<User>    usr;

    Wt::WDate transaction_date;
    std::string transaction_str;
    std::string transaction_tag;
    double  amount;
  private:
    std::string comment;
    Wt::WDate update_date;

};

#endif // Account_transaction_H
