#include <view/account_widget.h>
#include <view/combo_delegate.h>
//Account_widget::Account_widget() :
//WContainerWidget()
//{
//  //ctor
//}

//Account_widget::Account_widget(Wt::Dbo::backend::Postgres* p) :
Account_widget::Account_widget(Db_object_ptr dbp) :
WContainerWidget(),
dbobj(dbp)
{
  setup();
}


Account_widget::~Account_widget()
{
  //dtor
}

void Account_widget::setup()
{
 /*
  try
  {
    pdb = new Wt::Dbo::backend::Postgres("host=127.0.0.1 user=bidgoat password=etthemligtpassord port=5432 dbname=bidgoat");
    //pdb.connect("host=127.0.0.1 port=5432 user=bidgoat pass=etthemligtpassord dbname=bidgoat");
  }
  catch (Wt::Dbo::Exception& e)
  {
    return;
  }
  session.setConnection(*pdb);
  session.mapClass<Account_transaction>("account_transaction");
  session.mapClass<Account_chart>("account_chart");
  session.mapClass<Account>("account");
  session.mapClass<Tag>("tag");
  session.mapClass<User>("users");

  //Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account>>* qmodel;
  //qmodel = new Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account> >();
  qmodel = new Wt::Dbo::QueryModel<boost::tuple<Wt::Dbo::ptr<Account>, Wt::Dbo::ptr<Account_chart> > >();

  //qmodel->setQuery(session.find<Account>());
  qmodel->setQuery(session.query<boost::tuple<Wt::Dbo::ptr<Account>, Wt::Dbo::ptr<Account_chart> > >
  ("select acc, cht from account acc left join account_chart cht on acc.account_chart_id=cht.id"));
  //qmodel->addAllFieldsAsColumns();
  std::cerr << "Account widget:";
  for (auto f : qmodel->fields())
  {
    std::cerr << " " << f.name();
  }
  std::cerr << '\n';
*/
  auto qmodel = dbobj->qm_acc_acht;   // Copy pointer... stupid?

  for (auto qf = qmodel->fields().begin(); qf != qmodel->fields().end(); ++qf)
  {
    std::cerr << "F " << qf->name() << '\n';
  }

  qmodel->addColumn("acc.type", "Typ");
  qmodel->addColumn("acc.name", "Namn");
  qmodel->addColumn("acc.number", "Kontonummer");
  qmodel->addColumn("acc.account_chart_id");
  qmodel->addColumn("disp", "Konto");
  qmodel->addColumn("acc.desc_str", "Transaktionsinfo");
  qmodel->setColumnFlags(4,Wt::ItemIsSelectable | Wt::ItemIsEditable);
//  qmodel->addColumn("tag", "Tag");

  acc = new Wt::WTableView(this);
  acc->setModel(qmodel);
  acc->setAlternatingRowColors(true);
  acc->setColumnWidth(0,120);
  acc->setColumnWidth(1,360);
  acc->setColumnWidth(2,240);
  acc->setColumnWidth(3,120);
  acc->setColumnWidth(4,360);

  acc->setColumnHidden(3,true);

  acc->setSelectionMode(Wt::SelectionMode::ExtendedSelection);
  acc->selectionChanged().connect(this, &Account_widget::on_selection);

  acc->setEditTriggers(Wt::WAbstractItemView::EditTrigger::SingleClicked);
  acc->setEditOptions(Wt::WAbstractItemView::EditOption::SingleEditor);
  acc->setOverflow(WContainerWidget::OverflowScroll );
  acc->setHeight(Wt::WLength("700px")); // Det här är inte vackert - se till att få det dynamisk (via vanlig css) eller få containern att begränsa (som den borde...)

  Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_chart>>* chart_model; //= new Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_chart> >();
  chart_model = dbobj->qm_acht; //_selection;
  //chart_model->setQuery(session.find<Account_chart>());
//  chart_model->addColumn("id");
//  chart_model->addColumn("number", "Kontonummer");
//  chart_model->addColumn("name", "Namn");

//  std::cerr << chart_model->resultRow(1)->get_num_name();
  std::vector<Wt::WString>  acht;
  for (int i=0; i<chart_model->rowCount(); i++)
  {
    acht.push_back(Wt::WString(chart_model->resultRow(i)->get_num_name()));
  }

  Wt::WStringListModel* sl = new Wt::WStringListModel(acht);

  dbobj->qm_acc->addAllFieldsAsColumns();

  Combo_delegate* customdelegate = new Combo_delegate(qmodel, sl);
//  acc->setItemDelegate(customdelegate);
  acc->setItemDelegateForColumn(4,customdelegate);

  qmodel->dataChanged().connect(this, &Account_widget::on_data_changed);

  addWidget(acc);
}

void Account_widget::on_data_changed(const Wt::WModelIndex &index, const Wt::WModelIndex &index2)
{
  std::cerr << "On changed: " << index.row() << " " << index.column() << " : " << index2.row() << " " << index2.column() << '\n';
  dbobj->qm_acc_acht->reload();
}

void Account_widget::on_selection()
{
    const Wt::WModelIndexSet selectedRows = acc->selectedIndexes();
    for (Wt::WModelIndexSet::iterator i = selectedRows.begin(); i != selectedRows.end(); ++i) {
      std::cerr << "Index " << i->row() << '\n';
    }
}

/*
WWidget *ButtonDelegate::update(WWidget *widget, const WModelIndex& index, WFlags<ViewItemRenderFlag> flags)
{
  WPushButton *b;
  if (widget)
    b = dynamic_cast<WPushButton *>(widget);
  else
    b = new WPushButton();

  b->setText(asString(index.data(DisplayRole));

  return b;
}
*/
