#ifndef ACCOUNT_CHART_WIDGET_H
#define ACCOUNT_CHART_WIDGET_H

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

class Account_chart_widget : public Wt::WContainerWidget
{
  public:
    Account_chart_widget(Db_object_ptr dbp);
    //Account_widget(Wt::Dbo::backend::Postgres* p);
    virtual ~Account_chart_widget();
  protected:
    void setup();
    void on_selection();
    void add_account(short num, std::string name, std::string desc);

  private:
//    Wt::Dbo::backend::Postgres*  pdb;
//    Wt::Dbo::Session  session;

    Db_object_ptr dbobj;

    Wt::WTableView* acc;
//    Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_chart>>* qmodel;
};

#endif // ACCOUNT_WIDGET_H
