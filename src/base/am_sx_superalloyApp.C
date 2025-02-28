#include "am_sx_superalloyApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
am_sx_superalloyApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  return params;
}

am_sx_superalloyApp::am_sx_superalloyApp(InputParameters parameters) : MooseApp(parameters)
{
  am_sx_superalloyApp::registerAll(_factory, _action_factory, _syntax);
}

am_sx_superalloyApp::~am_sx_superalloyApp() {}

void 
am_sx_superalloyApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAllObjects<am_sx_superalloyApp>(f, af, s);
  Registry::registerObjectsTo(f, {"am_sx_superalloyApp"});
  Registry::registerActionsTo(af, {"am_sx_superalloyApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
am_sx_superalloyApp::registerApps()
{
  registerApp(am_sx_superalloyApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
am_sx_superalloyApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  am_sx_superalloyApp::registerAll(f, af, s);
}
extern "C" void
am_sx_superalloyApp__registerApps()
{
  am_sx_superalloyApp::registerApps();
}
