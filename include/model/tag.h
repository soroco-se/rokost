#ifndef TAG_H_INCLUDED
#define TAG_H_INCLUDED

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

class Account_transaction;

class Tag
{
  public:
  Tag() {}
  Tag(Tag& t)
  {
    tag_desc = t.tag_desc;
    tag_name = t.tag_name;
    account_transactions = t.account_transactions;
  }
  Tag(std::string n, std::string d) :
    tag_name(n),
    tag_desc(d)
  {}

  std::vector<std::string> get_field_names()
  {
    return { "name", "description"};
  }
  bool set_fields(std::vector<std::string>& f_vals)
  {
    if (f_vals.size() != 2)
      return false;
    tag_name = f_vals.at(0);
    tag_desc = f_vals.at(1);
    return true;
  }

  Wt::Dbo::collection< Wt::Dbo::ptr<Account_transaction> > account_transactions;

  template<class Action>
  void persist(Action& a)
  {
    Wt::Dbo::field(a,tag_name,"name");
    Wt::Dbo::field(a,tag_desc,"description");
    Wt::Dbo::hasMany(a, account_transactions, Wt::Dbo::ManyToMany, "account_transaction_tags");
  }

  private:
    std::string tag_name;
    std::string tag_desc;

};

#endif // TAG_H_INCLUDED
