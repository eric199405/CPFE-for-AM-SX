// Zixu Guo
// National University of Singapore
// Yilun Xu
// Imperial College London, IHPC A*STAR
// 01 MAY 2023

#include "CoupledVarDirichletBC.h"
#include "Function.h"

registerMooseObject("MooseApp", CoupledVarDirichletBC);

InputParameters
CoupledVarDirichletBC::validParams()
{
  InputParameters params = DirichletBCBase::validParams();
  params.addRequiredCoupledVar("v", "Coupled variable setting the value on the boundary.");
  params.addCoupledVar("scale_factor", 1., "Scale factor to multiply the heat flux with");
  params.addParam<Real>(
      "coef", 1.0, "Coefficent ($\\sigma$) multiplier for the coupled force term.");
  params.addRequiredParam<FunctionName>("function", "The forcing function.");
  params.addClassDescription("Imposes the essential boundary condition $u=v$, where $v$ "
                             "is a variable.");
  return params;
}

CoupledVarDirichletBC::CoupledVarDirichletBC(const InputParameters & parameters)
  : DirichletBCBase(parameters),
    _coupled_var(coupledValue("v")),
    _coupled_num(coupled("v")),
    _coef(getParam<Real>("coef")),
    _scale_factor(coupledValue("scale_factor")),
    _func(getFunction("function"))
{
}

Real
CoupledVarDirichletBC::computeQpValue()
{
  return  _coupled_var[_qp] + _func.value(_t, *_current_node);
}