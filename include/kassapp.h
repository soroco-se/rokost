#ifndef KASSAPP_H
#define KASSAPP_H

#include <memory>

#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>
#include <Wt/WApplication>

#include <model/db_objects.h>

using namespace Wt;

class Kassapp : public WApplication
{
  public:
    Kassapp(const WEnvironment& env);
    void run();

  protected:
    void setup();
    bool setup_db(bool create_tables=false);

  private:
    Wt::Dbo::backend::Postgres*  pdb;
    Wt::Dbo::Session  session;
    Db_object_ptr dbobj;
};

#endif // KASSAPP_H
