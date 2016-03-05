#include <view/account_chart_widget.h>

#include <cstdlib>

#include <Wt/WTable>
#include <Wt/WTableCell>
#include <Wt/WLineEdit>
#include <Wt/WIntValidator>
#include <Wt/WLengthValidator>
#include <Wt/WPushButton>
#include <Wt/WVBoxLayout>
//Account_widget::Account_widget() :
//WContainerWidget()
//{
//  //ctor
//}

//Account_widget::Account_widget(Wt::Dbo::backend::Postgres* p) :
Account_chart_widget::Account_chart_widget(Db_object_ptr dbp) :
WContainerWidget(),
dbobj(dbp)
{
  setup();
}


Account_chart_widget::~Account_chart_widget()
{
  //dtor
}

void Account_chart_widget::setup()
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
  session.mapClass<Account>("account");
  session.mapClass<Account_chart>("account_chart");
  session.mapClass<Tag>("tag");
  session.mapClass<User>("users");

  //Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account>>* qmodel;
  qmodel = new Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account_chart> >();
  qmodel->setQuery(session.find<Account_chart>());
  //qmodel->addAllFieldsAsColumns();

  for (auto f : qmodel->fields())
  {
    std::cerr << " " << f.name();
  }
  std::cerr << '\n';
*/
  auto qmodel = dbobj->qm_acht;
  qmodel->addColumn("number", "Kontonummer");
  qmodel->addColumn("name", "Namn");
  qmodel->addColumn("description", "Beskrivning");
//  qmodel->setColumnFlags(3,Wt::ItemIsSelectable | Wt::ItemIsEditable);
//  qmodel->addColumn("tag", "Tag");

  acc = new Wt::WTableView(this);
  acc->setModel(qmodel);
  acc->setAlternatingRowColors(true);
  acc->setColumnWidth(0,240);
  acc->setColumnWidth(1,360);
  acc->setColumnWidth(2,360);

  acc->setSelectionMode(Wt::SelectionMode::ExtendedSelection);
  acc->selectionChanged().connect(this, &Account_chart_widget::on_selection);

  acc->setEditTriggers(Wt::WAbstractItemView::EditTrigger::DoubleClicked);
  acc->setEditOptions(Wt::WAbstractItemView::EditOption::SingleEditor);
  acc->setOverflow(WContainerWidget::OverflowScroll );
  acc->setHeight(Wt::WLength("700px")); // Det här är inte vackert - se till att få det dynamisk (via vanlig css) eller få containern att begränsa (som den borde...)
  //acc->setItemDelegateForColumn()
  //addWidget(acc);

  // Add form
  Wt::WTable *add_form = new Wt::WTable();
  add_form->setHeaderCount(0);
  add_form->setStyleClass("form-group");

//  add_form->elementAt(0, 0)->addWidget(new Wt::WText("Kontonummer"));
//  add_form->elementAt(0, 1)->addWidget(new Wt::WText("Namn"));
//  add_form->elementAt(0, 2)->addWidget(new Wt::WText("Beskrivning"));

  Wt::WLineEdit *num = new Wt::WLineEdit();
  num->setPlaceholderText("Kontonummer");
  num->setValidator(new Wt::WIntValidator(0,9999));
  add_form->elementAt(0,0)->addWidget(num);

  Wt::WLineEdit *name = new Wt::WLineEdit();
  name->setPlaceholderText("Namn");
  name->setValidator(new Wt::WLengthValidator(3,80));
  add_form->elementAt(0,1)->addWidget(name);

  Wt::WLineEdit *desc = new Wt::WLineEdit();
  desc->setPlaceholderText("Beskrivning");
  desc->setValidator(new Wt::WLengthValidator(3,80));
  add_form->elementAt(0,2)->addWidget(desc);

  Wt::WText *info = new Wt::WText();
  info->setInline(false);
  info->hide();

  Wt::WPushButton *button = new Wt::WPushButton("Lägg till");
  button->clicked().connect(std::bind([=] () {
    bool all_ok = (num->validate() == Wt::WValidator::Valid);
    all_ok &= (name->validate() == Wt::WValidator::Valid);
    all_ok &= (desc->validate() == Wt::WValidator::Valid);
    if (all_ok)
    {
      short n = std::atoi(num->text().narrow().c_str());
      add_account(n, name->text().toUTF8(), desc->text().toUTF8());
      num->setFocus();
      num->setText("");
      name->setText("");
      desc->setText("");
      info->hide();
    }
    else
    {
      info->setText("Alla fält måste fyllas i, kontonummer mellan 0 och 9999, namn och beskrivning längre än 3 tecken.");
      info->setStyleClass("alert");
      info->show();
    }
  } ));

  add_form->elementAt(0,3)->addWidget(button);

  add_form->elementAt(1,0)->setColumnSpan(4);
  add_form->elementAt(1,0)->addWidget(info);

  Wt::WVBoxLayout *vbox = new Wt::WVBoxLayout();
  setLayout(vbox);

  vbox->addWidget(acc);
  vbox->addWidget(add_form);
}

void Account_chart_widget::add_account(short num, std::string name, std::string desc)
{
  std::cerr << "Add_account " << num << " " << name << " " << desc << '\n';
  auto qmodel = dbobj->qm_acht;
  int row = qmodel->rowCount();
  qmodel->insertRow(row);
  qmodel->setData(row, 0, num);
  qmodel->setData(row, 1, name);
  qmodel->setData(row, 2, desc);
  acc->select(qmodel->index(row,0));
}


void Account_chart_widget::on_selection()
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
