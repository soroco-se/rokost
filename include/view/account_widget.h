#ifndef ACCOUNT_WIDGET_H
#define ACCOUNT_WIDGET_H

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WContainerWidget>

#include <model/account_transaction.h>
#include <model/account.h>
#include <model/account_chart.h>
#include <model/tag.h>
#include <model/user.h>
#include <model/db_objects.h>

#include <Wt/Dbo/QueryModel>
#include <Wt/WTableView>


class Account_widget : public Wt::WContainerWidget
{
  public:
    Account_widget(Db_object_ptr dbp);
    //Account_widget(Wt::Dbo::backend::Postgres* p);
    virtual ~Account_widget();
  protected:
    void setup();
    void on_selection();
    void on_data_changed(const Wt::WModelIndex &index, const Wt::WModelIndex &index2);

  private:
//    Wt::Dbo::backend::Postgres*  pdb;
//    Wt::Dbo::Session  session;

    Db_object_ptr dbobj;

    Wt::WTableView* acc;
    //Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account>>* qmodel;
    //Wt::Dbo::QueryModel<boost::tuple<Wt::Dbo::ptr<Account>, Wt::Dbo::ptr<Account_chart> > >* qmodel;
};

#endif // ACCOUNT_WIDGET_H
