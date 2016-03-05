#ifndef USER_H_INCLUDED
#define USER_H_INCLUDED

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/WtSqlTraits>

class User
{
  public:
    User() {}
    User(User& u)
    {
      type = u.type;
      name = u.name;
      uid = u.uid;
    }
    User(std::string t, std::string n, std::string i) : type(t), name(n), uid(i)
    {}

    std::vector<std::string> get_field_names();     // Returns the fields names
    bool set_fields(std::vector<std::string>& f_vals);  // In the order returned by get_field_names

    template <class Action>
    void persist(Action &a)
    {
      Wt::Dbo::field(a, uid, "uid");
      Wt::Dbo::field(a, name, "name");
      Wt::Dbo::field(a, type, "type");
      Wt::Dbo::hasMany(a, updated_transactions, Wt::Dbo::ManyToOne, "updated_transactions");
    }

    Wt::Dbo::collection<Wt::Dbo::ptr<Account_transaction> > updated_transactions;

  private:
    std::string type;   // Regular user, admin
    std::string name;   // Full name
    std::string uid;    // userid
};

// Oh, divine lord please forgive me
inline std::vector<std::string> User::get_field_names()
{
    return {"uid", "name", "type"};
}

inline bool User::set_fields(std::vector<std::string>& f_vals)
{
  if (f_vals.size() != 3)
    return false;
  uid = f_vals.at(0);
  name = f_vals.at(1);
  type  = f_vals.at(2);
  return true;
}


#endif // USER_H_INCLUDED
