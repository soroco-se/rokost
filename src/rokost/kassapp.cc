#include <iostream>

#include <kassapp.h>
#include <model/account_transaction.h>
#include <model/account.h>
#include <model/tag.h>
#include <model/user.h>

#include <Wt/Dbo/QueryModel>
#include <Wt/WTableView>

#include <Wt/WContainerWidget>
#include <Wt/WBorderLayout>
#include <Wt/WText>
#include <Wt/WMenu>
#include <Wt/WStackedWidget>
#include <Wt/WTextArea>

#include <view/account_widget.h>
#include <view/account_chart_widget.h>
#include <view/transactions_widget.h>


Kassapp::Kassapp(const WEnvironment& env)
  : WApplication(env)
{
  setup_db();
  setup();


  //run();
}

void Kassapp::setup()
{
  Wt::WContainerWidget *contall = root();

  Wt::WBorderLayout *layout = new Wt::WBorderLayout();
  contall->setLayout(layout);

  // Design top row (add status info, login and so on and soforth
  Wt::WText *item = new Wt::WText("KassApp",Wt::PlainText);
  item->setStyleClass("top-row");
  item->setToolTip("En kassabok",  Wt::XHTMLText);
  layout->addWidget(item, Wt::WBorderLayout::North);

  // Menu
  Wt::WContainerWidget *mcontainer = new Wt::WContainerWidget();

  // Create a stack where the contents will be located.
  Wt::WStackedWidget *mcontents = new Wt::WStackedWidget();

  Wt::WMenu *menu = new Wt::WMenu(mcontents, mcontainer);
  menu->setStyleClass("nav nav-pills nav-stacked");
  menu->setWidth(175);

  // Add menu items using the default lazy loading policy.
  menu->addItem("Översikt", new Wt::WTextArea("En översikt över konton och utgiftsområden med fina grafer o sånt"));
//  menu->addItem("Nya händelser", new Wt::WTextArea("Nya, dvs obekräftade, kontohändelser (från bearbetade kontoutdrag)"));
  menu->addItem(Wt::WString("Nya Händelser"),new Transactions_widget(dbobj));
  menu->addItem("Gruppera konton", new Account_widget(dbobj));  // Slå ihop virtuella konton
  menu->addItem("Kontoplan", new Account_chart_widget(dbobj));
//  menu->addItem("Tab widget", new Wt::WTextArea("Tab widget contents"));
//  menu->addItem("Menu", new Wt::WTextArea("Menu contents"));

  // Add menu to West
  layout->addWidget(mcontainer, Wt::WBorderLayout::West);

  // Add content to Center
  layout->addWidget(mcontents, Wt::WBorderLayout::Center);

  // Ignore East
  //layout->addWidget(item, Wt::WBorderLayout::East);

  // Might use a footer
  Wt::WContainerWidget *fcontainer = new Wt::WContainerWidget();
  item = new Wt::WText("KassApp - En kassabok, Jakob 2016");
  fcontainer->addWidget(item);
  fcontainer->setContentAlignment(Wt::AlignCenter);
  fcontainer->setPadding(Wt::WLength::Auto, Wt::Left|Wt::Right);
  layout->addWidget(item, Wt::WBorderLayout::South);


}

void Kassapp::run()
{

  Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_transaction>>* qmodel;
  qmodel = new Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_transaction> >();
  qmodel->setQuery(session.find<Account_transaction>());
  //qmodel->addAllFieldsAsColumns();

  for (auto f : qmodel->fields())
  {
    std::cerr << " " << f.name();
  }
  std::cerr << '\n';
  qmodel->addColumn("date", "Datum");
  qmodel->addColumn("description", "Beskrivning");
  qmodel->addColumn("amount", "Belopp");
//  qmodel->addColumn("tag", "Tag");

  Wt::WTableView* acctrans = new WTableView(root());
  acctrans->setModel(qmodel);
  acctrans->setAlternatingRowColors(true);
  acctrans->setColumnWidth(0,120);
  acctrans->setColumnWidth(1,360);
  acctrans->setColumnWidth(2,240);
//  acctrans->setColumnWidth(3,360);
  acctrans->setSelectionMode(Wt::SelectionMode::ExtendedSelection);
  root()->addWidget(acctrans);
}



bool Kassapp::setup_db(bool create_tables)
{
  try
  {
    pdb = new Wt::Dbo::backend::Postgres("host=127.0.0.1 user=bidgoat password=etthemligtpassord port=5432 dbname=bidgoat");
    //pdb.connect("host=127.0.0.1 port=5432 user=bidgoat pass=etthemligtpassord dbname=bidgoat");
  }
  catch (Wt::Dbo::Exception& e)
  {
    std::cerr << "Failed to connect to database " << e.code() << '\n';
    return false;
  }
  session.setConnection(*pdb);
  session.mapClass<Account_transaction>("account_transaction");
  session.mapClass<Account_chart>("account_chart");
  session.mapClass<Account>("account");
  session.mapClass<Tag>("tag");
  session.mapClass<User>("users");

  if (create_tables)
  {
    try
    {
      session.createTables();
    }
    catch (Wt::Dbo::Exception& e)
    {
      std::cerr << "Tables exist " << e.code() << '\n';
      return false;
    }
  }
  // Create all queries used in the app
  dbobj.reset(new Db_objects());

  // Query Account and Account_chart for informative output...
//  dbobj->qm_acc_acht = new Wt::Dbo::QueryModel<boost::tuple<Wt::Dbo::ptr<Account>, Wt::Dbo::ptr<Account_chart> > >();
  dbobj->qm_acc_acht = new Wt::Dbo::QueryModel<boost::tuple<Wt::Dbo::ptr<Account>, std::string > >();
//  dbobj->qm_acc_acht->setQuery(session.query<boost::tuple<Wt::Dbo::ptr<Account>, Wt::Dbo::ptr<Account_chart> > >
  dbobj->qm_acc_acht->setQuery(session.query<boost::tuple<Wt::Dbo::ptr<Account>, std::string > >
                      ("select acc, (account_chart.number || ' - ' || account_chart.name) as disp from account acc left join account_chart on acc.account_chart_id=account_chart.id"));

  // Query Account_chart for all rows
  dbobj->qm_acht = new Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_chart> >();
  dbobj->qm_acht->setQuery(session.find<Account_chart>());

  dbobj->qm_acc = new Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account> >();
  dbobj->qm_acc->setQuery(session.find<Account>());

  dbobj->qm_trans = new Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_transaction> >();
//  dbobj->qm_trans->setQuery(session.find<Account_transaction>());
  dbobj->qm_trans->setQuery(session.query<Wt::Dbo::ptr<Account_transaction> >
                      ("select act from account_transaction act where act.updated is null"));

  return true;
}
