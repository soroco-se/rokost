#ifndef COMBO_DELEGATE_H_INCLUDED
#define COMBO_DELEGATE_H_INCLUDED

#include <Wt/WStandardItem>
#include <Wt/WStandardItemModel>
#include <Wt/WStringListModel>
#include <Wt/WTableView>
#include <Wt/WItemDelegate>
#include <Wt/WContainerWidget>
#include <Wt/WComboBox>

/*
 * This delegate demonstrates how to override the editing behaviour of a
 * table cell.
 *
 * It takes a list of possible items on construction and, when edited, saves
 * the selected item from the list to the Wt::DisplayRole in the model for
 * Wt::WItemDelegate to render.
 * It also saves the items index for future editing (rather than each time
 * searching the item in the list). This is done using the general purpose
 * Wt::UserRole in the model.
 */
class Combo_delegate : public Wt::WItemDelegate
{
public:
  Combo_delegate(Wt::WAbstractItemModel* model_to_update, Wt::WAbstractItemModel* model_to_display)
    : update(model_to_update), display(model_to_display)
  { }

  void setModelData(const boost::any &editState, Wt::WAbstractItemModel* model,
                    const Wt::WModelIndex &index) const
  {
    int stringIdx = (int)Wt::asNumber(editState);
    std::cerr << "Save model data " << stringIdx << '\n';   // Det här är valet i comboboxen
    std::cerr << "Save index " << index.row() << " , " << index.column() << '\n'; // Det här är vlad cell i vyn

    bool ok = update->setData(index.row(), 3, stringIdx+1, Wt::EditRole); // Postgresql är 1-baserad....
    std::cerr << "Update " << (ok ? "true":"false") << '\n';
    update->dataChanged().emit(update->index(0,0),update->index(update->rowCount()-1,update->columnCount()-1));

//    update->setData(index, )
// -> this one    bool ok = update->setData(index, col, val);
//    model->setData(index, stringIdx, Wt::UserRole);
//    model->setData(index, items_->data(stringIdx, 0), Wt::DisplayRole);
  }

  boost::any editState(Wt::WWidget* editor) const
  {
    Wt::WComboBox* combo = dynamic_cast<Wt::WComboBox*>
                           (dynamic_cast<Wt::WContainerWidget*>(editor)->widget(0));
    return combo->currentIndex();
  }

  void setEditState(Wt::WWidget* editor, const boost::any &value) const
  {
    Wt::WComboBox* combo = dynamic_cast<Wt::WComboBox*>
                           (dynamic_cast<Wt::WContainerWidget*>(editor)->widget(0));
    combo->setCurrentIndex((int)Wt::asNumber(value));
  }

protected:
  virtual Wt::WWidget* createEditor(const Wt::WModelIndex &index,
                                    Wt::WFlags<Wt::ViewItemRenderFlag> flags) const
  {
    Wt::WContainerWidget *const container = new Wt::WContainerWidget();
    Wt::WComboBox* combo = new Wt::WComboBox(container);

    std::cerr << "Incoming index " << index.row() << " , " << index.column() << '\n';

    combo->setModel(display);
/*
    const Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account>>* acc = static_cast<const Wt::Dbo::QueryModel<Wt::Dbo::ptr<Account> > *>(index.model());

    std::cerr << "Type at col 3 " << acc->data(index.row(),3).type().name() << '\n';
    int id=0;
    try{
      auto i = boost::any_cast<int64_t>(acc->data(index.row(),3));
      id = static_cast<int>(i);
    }
    catch (boost::bad_any_cast& ba)
    {
      std::cerr << "Bad any " << ba.what() << '\n';
    }

    combo->setCurrentIndex(id);
*/
    combo->changed().connect(boost::bind(&Combo_delegate::doCloseEditor, this,
                                         container, true));
    combo->enterPressed().connect(boost::bind(&Combo_delegate::doCloseEditor,
                                  this, container, true));
    combo->escapePressed().connect(boost::bind(&Combo_delegate::doCloseEditor,
                                   this, container, false));

    return container;
  }

private:
  Wt::WAbstractItemModel* update;
  Wt::WAbstractItemModel* display;

  void doCloseEditor(Wt::WWidget *editor, bool save) const
  {
    closeEditor().emit(editor, save);
  }
};



#endif // COMBO_DELEGATE_H_INCLUDED
