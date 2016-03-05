#include <iostream>

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WContainerWidget>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>
#include <functional>

#include <Wt/WBootstrapTheme>

#include "kassapp.h"

using namespace Wt;

WApplication *createApplication(const WEnvironment& env)
{
  Wt::WApplication* app = new Kassapp(env);
   Wt::WBootstrapTheme *bootstrapTheme = new Wt::WBootstrapTheme(app);
    bootstrapTheme->setVersion(Wt::WBootstrapTheme::Version3);
    bootstrapTheme->setResponsive(true);
    app->setTheme(bootstrapTheme);

    // load the default bootstrap3 (sub-)theme
    app->useStyleSheet("resources/themes/bootstrap/3/bootstrap-theme.min.css");
//    app->useStyleSheet("css/kassapp.css");

    app->setTitle("KassApp 0.1");


  return app;
}

int main(int argc, char** argv)
{
  Wt::WString::setDefaultEncoding(Wt::CharEncoding::UTF8);
  return WRun(argc, argv, &createApplication);
}
