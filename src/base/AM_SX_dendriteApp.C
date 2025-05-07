#include "AM_SX_dendriteApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "ModulesApp.h"
#include "MooseSyntax.h"

InputParameters
AM_SX_dendriteApp::validParams()
{
  InputParameters params = MooseApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;
  return params;
}

AM_SX_dendriteApp::AM_SX_dendriteApp(InputParameters parameters) : MooseApp(parameters)
{
  AM_SX_dendriteApp::registerAll(_factory, _action_factory, _syntax);
}

AM_SX_dendriteApp::~AM_SX_dendriteApp() {}

void
AM_SX_dendriteApp::registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  ModulesApp::registerAllObjects<AM_SX_dendriteApp>(f, af, s);
  Registry::registerObjectsTo(f, {"AM_SX_dendriteApp"});
  Registry::registerActionsTo(af, {"AM_SX_dendriteApp"});

  /* register custom execute flags, action syntax, etc. here */
}

void
AM_SX_dendriteApp::registerApps()
{
  registerApp(AM_SX_dendriteApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
extern "C" void
AM_SX_dendriteApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  AM_SX_dendriteApp::registerAll(f, af, s);
}
extern "C" void
AM_SX_dendriteApp__registerApps()
{
  AM_SX_dendriteApp::registerApps();
}
