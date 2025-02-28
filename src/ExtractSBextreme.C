
#include "ExtractSBextreme.h"

registerMooseObject("MooseApp", ExtractSBextreme);

InputParameters
ExtractSBextreme::validParams()
{
  InputParameters params = ExtremeValueBase<ElementVariablePostprocessor>::validParams();
  params.addRequiredCoupledVar("inside_sb_region", "-");
  params.addCoupledVar("proxy_variable",
                       "The name of the variable to use to identify the location at which "
                       "the variable value should be taken; if not provided, this defaults "
                       "to the 'variable'.");
  params.addClassDescription(
      "Finds either the min or max elemental value of a variable over the domain.");
  return params;
}

ExtractSBextreme::ExtractSBextreme(const InputParameters & parameters)
  : ExtremeValueBase<ElementVariablePostprocessor>(parameters),
    _inside_sb_region(coupledValue("inside_sb_region")),
    _proxy_variable(isParamValid("proxy_variable") ? coupledValue("proxy_variable") : _u)
{
  if (this->isNodal())
    this->paramError("variable", "This AuxKernel only supports Elemental fields");
  _use_proxy = isParamValid("proxy_variable");
}

std::pair<Real, Real>
ExtractSBextreme::getProxyValuePair()
{
  switch (_type)
  {
    case ExtremeType::MAX:
         if (_inside_sb_region[_qp]==1 && abs(_q_point[_qp](1))<0.15) return std::make_pair(_proxy_variable[_qp], _u[_qp]);
         else return std::make_pair(1E-10, 1E-10);
    case ExtremeType::MIN:
         if (_inside_sb_region[_qp]==1 && abs(_q_point[_qp](1))<0.15) return std::make_pair(_proxy_variable[_qp], _u[_qp]);
         else return std::make_pair(1E+100, 1E+100);
  }
  return std::make_pair(_proxy_variable[_qp], _u[_qp]);
}
