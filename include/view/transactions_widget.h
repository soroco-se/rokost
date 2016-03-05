#ifndef TRANSACTIONS_WIDGET_H
#define TRANSACTIONS_WIDGET_H

#include <model/db_objects.h>

#include <Wt/Dbo/QueryModel>
#include <Wt/WTableView>

class Transactions_widget: public Wt::WContainerWidget
{
  public:
    Transactions_widget(Db_object_ptr dbp);
    virtual ~Transactions_widget();
  protected:
    void setup();

  private:
      Db_object_ptr dbobj;

};

#endif // TRANSACTIONS_WIDGET_H
