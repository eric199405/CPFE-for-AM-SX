//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html
#include "AM_SX_dendriteTestApp.h"
#include "AM_SX_dendriteApp.h"
#include "Moose.h"
#include "AppFactory.h"
#include "MooseSyntax.h"

InputParameters
AM_SX_dendriteTestApp::validParams()
{
  InputParameters params = AM_SX_dendriteApp::validParams();
  params.set<bool>("use_legacy_material_output") = false;
  params.set<bool>("use_legacy_initial_residual_evaluation_behavior") = false;
  return params;
}

AM_SX_dendriteTestApp::AM_SX_dendriteTestApp(InputParameters parameters) : MooseApp(parameters)
{
  AM_SX_dendriteTestApp::registerAll(
      _factory, _action_factory, _syntax, getParam<bool>("allow_test_objects"));
}

AM_SX_dendriteTestApp::~AM_SX_dendriteTestApp() {}

void
AM_SX_dendriteTestApp::registerAll(Factory & f, ActionFactory & af, Syntax & s, bool use_test_objs)
{
  AM_SX_dendriteApp::registerAll(f, af, s);
  if (use_test_objs)
  {
    Registry::registerObjectsTo(f, {"AM_SX_dendriteTestApp"});
    Registry::registerActionsTo(af, {"AM_SX_dendriteTestApp"});
  }
}

void
AM_SX_dendriteTestApp::registerApps()
{
  registerApp(AM_SX_dendriteApp);
  registerApp(AM_SX_dendriteTestApp);
}

/***************************************************************************************************
 *********************** Dynamic Library Entry Points - DO NOT MODIFY ******************************
 **************************************************************************************************/
// External entry point for dynamic application loading
extern "C" void
AM_SX_dendriteTestApp__registerAll(Factory & f, ActionFactory & af, Syntax & s)
{
  AM_SX_dendriteTestApp::registerAll(f, af, s);
}
extern "C" void
AM_SX_dendriteTestApp__registerApps()
{
  AM_SX_dendriteTestApp::registerApps();
}
