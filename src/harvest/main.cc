#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <model/account_transaction.h>
#include <model/account.h>
#include <model/account_chart.h>
#include <csv_reader.h>
#include <model/tag.h>
#include <model/user.h>
#include <Wt/Dbo/Dbo>
#include <Wt/Dbo/backend/Postgres>

#include <glob.h>
#include <tclap/CmdLine.h>

#ifndef VERSION
#define VERSION "0.0.2"
#endif

using namespace std;
using namespace Util;

Wt::Dbo::backend::Postgres*  pdb;
Wt::Dbo::Session  session;

void setup_db()
{
  try
  {
    pdb = new Wt::Dbo::backend::Postgres("host=127.0.0.1 user=bidgoat password=etthemligtpassord port=5432 dbname=bidgoat");
    //pdb.connect("host=127.0.0.1 port=5432 user=bidgoat pass=etthemligtpassord dbname=bidgoat");
  }
  catch (Wt::Dbo::Exception& e)
  {
    cerr << "Sket sig " << e.code();
    exit(EXIT_FAILURE);
  }
  session.setConnection(*pdb);
  session.mapClass<Account_transaction>("account_transaction");
  session.mapClass<Account_chart>("account_chart");
  session.mapClass<Account>("account");
  session.mapClass<Tag>("tag");
  session.mapClass<User>("users");
  try
  {
    session.createTables();
  }
  catch (Wt::Dbo::Exception& e)
  {
    cerr << "Tables exist " << e.code() << '\n';
  }

}

void add_to_db(Account_transaction& a, Wt::Dbo::ptr<Account> dbt, Wt::Dbo::ptr<Account> cdt, Wt::Dbo::ptr<Tag> tag)
{
  Wt::Dbo::Transaction  trans(session);
//  Account_transaction* acc = new Account_transaction(a);
//  Wt::Dbo::ptr<Account_transaction> accPtr = session.add(acc);
  Wt::Dbo::ptr<Account_transaction> accPtr = session.add(new Account_transaction(a));

  accPtr.modify()->dbt = dbt;
  accPtr.modify()->cdt = cdt;
  if (tag)
    accPtr.modify()->tags.insert(tag);
  trans.commit();
}


std::vector<std::string> find_files(std::string dir)
{
  glob_t globbuf;
  std::stringstream ss;
  ss << dir << "/*.csv";
  int ret = glob(ss.str().c_str(), GLOB_ERR | GLOB_TILDE, NULL, &globbuf);
  if (ret != 0)
  {
    switch(ret)
    {
      case GLOB_ABORTED: std::cerr << "Read error while reading dir: " << dir << '\n'; break;
      case GLOB_NOMATCH: std::cerr << "Hittade inga filer som matchade mönstret *.csv i katalogen " << dir << " " << ss.str() << '\n'; break;
      default:  std::cerr << "Something didn't go as planned...\n"; break;
    }
    return std::vector<std::string>();
  }
  std::vector<std::string> files;
  for (size_t i=0; i<globbuf.gl_pathc; i++)
  {
    files.push_back(globbuf.gl_pathv[i]);
  }
  globfree(&globbuf);
  return files;
}

bool get_tag_and_target(std::string trans, std::string& tag_str, std::string& desc_str)
{
  size_t pos = trans.find_first_of(' ');
  if (pos == std::string::npos)
    return false;
  tag_str = trans.substr(0,pos);
  if (pos == trans.size())
    return false;
  desc_str = trans.substr(pos+1);
  if (tag_str.compare("Kortköp") == 0)    // Kortköp 100907 ST1. där 100907 är datum
  {
    pos = trans.find_first_not_of("0123456789 ",pos+1);
    desc_str = trans.substr(pos);
  }
  return true;
}

int main(int argc, char** argv)
{
  string filename;
  string dirname;
  string account_name;
  try
  {
    TCLAP::CmdLine cmd("Bidgoat command line interface", ' ', VERSION);
    TCLAP::ValueArg<string> csv_file_arg("f", "fil", "Kontoudrag i csv-format", true, "0901.csv", "");
    TCLAP::ValueArg<string> account_arg("k", "konto","Namn på kontot i databasen", true, "Huskontot", "Huskontot");
    TCLAP::ValueArg<string> csv_dir_arg("d", "dir", "Katalog med kontoutdrag i csv-format", true, "~/csv/konto", "");

    cmd.add(account_arg);
    cmd.xorAdd(csv_file_arg, csv_dir_arg);

    cmd.parse(argc, argv);

    if (csv_file_arg.isSet())
    {
        filename = csv_file_arg.getValue();
    }
    if (csv_dir_arg.isSet())
    {
        dirname = csv_dir_arg.getValue();
    }
    if (account_arg.isSet())
    {
      account_name = account_arg.getValue();
    }
  }
  catch (TCLAP::ArgException &e)
  {
    std::cerr << "Parse error " << e.error() << " for arg " << e.argId() << '\n';
  }

  setup_db();

  Wt::Dbo::Transaction  trans(session);
  Wt::Dbo::ptr<Account> accp = session.find<Account>().where("LOWER(name) = LOWER(?)").bind(account_name);
  trans.commit();

  if (!accp)
  {
    std::cerr << "Det finns inget konto som heter '" << account_name << "' i databasen.\n";
    exit(EXIT_FAILURE);
  }

  std::vector<std::string> files;
  if (!dirname.empty())
  {
    files = find_files(dirname);
  }
  else if (!filename.empty())
  {
    files.push_back(filename);
  }
  else
  {
    std::cerr << "Både katalognamn och filnamn är tomma - hur gick det till?\n";
    exit(EXIT_FAILURE);
  }

  for (auto f=files.begin();f!=files.end();++f)
  {
    CSV_reader csvr;
    if (csvr.open(*f))
    {
      std::cout << "Lägger till transaktioner från " << *f << " till databasen.\n";
        while (csvr.has_more())
        {
            Account_transaction this_trans;
            if (this_trans.set(csvr.get_row()))
            {
              // Baserat på 'description', sök efter ett account som matchar
              // Om inget hittas, skapa ett!
              // Använd första ordet som tag (ex. Betalning) och resten används som söksträng mot Account.
              // På så vis blir det tillslut autotaggat på de vanligaste =)

              Wt::Dbo::ptr<Account> otherp;
              Wt::Dbo::ptr<Tag> tagp;
              std::string tag_str, desc_str;
              if (get_tag_and_target(this_trans.transaction_str, tag_str, desc_str))
              {
                // First try to find an existing account
                Wt::Dbo::Transaction  trans(session);
                otherp = session.find<Account>().where("desc_str = ?").bind(desc_str);
                trans.commit();
                if (!otherp)
                {
                  // Add a new account
                  Wt::Dbo::Transaction  t(session);
                  Account* tmp_acc = new Account(desc_str);
                  otherp = session.add(tmp_acc);
                  t.commit();
                }
                // Try to find an existing tag
                Wt::Dbo::Transaction  trans_tag(session);
                tagp = session.find<Tag>().where("LOWER(name) = LOWER(?)").bind(tag_str);
                trans_tag.commit();
                if (!tagp)
                {
                  // Add a new tag
                  Wt::Dbo::Transaction  t(session);
                  Tag* tmp_tag = new Tag(tag_str, "");
                  tagp = session.add(tmp_tag);
                  t.commit();
                }
              }
              if (this_trans.is_debit())
              {
//                add_to_db(this_trans, accp, Wt::Dbo::ptr<Account>());
                add_to_db(this_trans, accp, otherp, tagp);
              }
              else
              {
                add_to_db(this_trans, otherp, accp, tagp);
//                add_to_db(this_trans, Wt::Dbo::ptr<Account>(), accp);
              }
 //             cout << "Line: " << " added to db.\n";
            }
        }
    }
    else
    {
      std::cerr << "Failed to open file " << *f << '\n';
    }
  }

  Wt::Dbo::Transaction  tran(session);
//  Wt::Dbo::ptr<Account> husp2 = session.find<Account>().where("name = ?").bind("huskontot");
//  cout << "Size: " << husp2->dbt_transactions.size() << '\n';
//  cout << "Size: " << husp2->cdt_transactions.size() << '\n';
  cout << "Saldo: " <<accp->get_balance() << '\n';

  return 0;
}
