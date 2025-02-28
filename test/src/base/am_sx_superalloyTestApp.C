//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "am_sx_superalloyTestApp.h"
#include "am_sx_superalloyApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

InputParameters
am_sx_superalloyTestApp::validParams()
{
  InputParameters params = am_sx_superalloyApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  return params;
}

am_sx_superalloyTestApp::am_sx_superalloyTestApp(InputParameters parameters) : MooseApp(parameters)
{
  am_sx_superalloyTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

am_sx_superalloyTestApp::~am_sx_superalloyTestApp() {}

void
am_sx_superalloyTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  am_sx_superalloyApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"am_sx_superalloyTestApp"});
    Registry::registerActionsTo(af, {"am_sx_superalloyTestApp"});
  }
}

void
am_sx_superalloyTestApp::registerApps()
{
  registerApp(am_sx_superalloyApp);
  registerApp(am_sx_superalloyTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
am_sx_superalloyTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  am_sx_superalloyTestApp::registerAll(f, af, s);
}
extern "C" void
am_sx_superalloyTestApp__registerApps()
{
  am_sx_superalloyTestApp::registerApps();
}
