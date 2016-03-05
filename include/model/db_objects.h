#ifndef DB_OBJECTS_H_INCLUDED
#define DB_OBJECTS_H_INCLUDED

#include <Wt/Dbo/Dbo>

#include <model/account_transaction.h>
#include <model/account.h>
#include <model/account_chart.h>
#include <model/tag.h>
#include <model/user.h>

struct Db_objects
{
//  Wt::Dbo::QueryModel<boost::tuple<Wt::Dbo::ptr<Account>, Wt::Dbo::ptr<Account_chart> > >* qm_acc_acht;
  Wt::Dbo::QueryModel<boost::tuple<Wt::Dbo::ptr<Account>, std::string > >* qm_acc_acht;
  Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_chart>>* qm_acht;
  Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account>>* qm_acc;
  Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_transaction> >* qm_trans;
};

typedef std::shared_ptr<Db_objects> Db_object_ptr;

#endif // DB_OBJECTS_H_INCLUDED
