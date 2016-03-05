#include <view/transactions_widget.h>

Transactions_widget::Transactions_widget(Db_object_ptr dbp) :
WContainerWidget(),
dbobj(dbp)
{
  setup();
}

Transactions_widget::~Transactions_widget()
{
  //dtor
}

void Transactions_widget::setup()
{
  auto qmodel = dbobj->qm_trans;
  for (auto qf = qmodel->fields().begin(); qf != qmodel->fields().end(); ++qf)
  {
    std::cerr << "F " << qf->name() << '\n';
  }

  //qmodel->addAllFieldsAsColumns();
  qmodel->addColumn("description");

  qmodel->setColumnFlags(1,Wt::ItemIsSelectable | Wt::ItemIsUserCheckable);


  Wt::WTableView* acc = new Wt::WTableView(this);
  acc->setModel(qmodel);
  acc->setAlternatingRowColors(true);
  acc->setColumnWidth(0,240);
  acc->setColumnWidth(1,360);
  acc->setSelectionMode(Wt::SingleSelection);
//  acc->setColumnWidth(2,360);


/*
  acc->setSelectionMode(Wt::SelectionMode::ExtendedSelection);
  acc->selectionChanged().connect(this, &Account_chart_widget::on_selection);

  acc->setEditTriggers(Wt::WAbstractItemView::EditTrigger::DoubleClicked);
  acc->setEditOptions(Wt::WAbstractItemView::EditOption::SingleEditor);
*/
  acc->setOverflow(WContainerWidget::OverflowScroll );
  acc->setHeight(Wt::WLength("700px")); // Det här är inte vackert - se till att få det dynamisk (via vanlig css) eller få containern att begränsa (som den borde...)
  addWidget(acc);
}
