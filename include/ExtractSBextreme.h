#pragma once
#include "ElementVariablePostprocessor.h"
#include "ExtremeValueBase.h"

/// A postprocessor for collecting the elemental min or max value
class ExtractSBextreme : public ExtremeValueBase<ElementVariablePostprocessor>
{
public:
  static InputParameters validParams();

  ExtractSBextreme(const InputParameters & parameters);

protected:
  virtual std::pair<Real, Real> getProxyValuePair() override;

  virtual void computeQpValue() override { computeExtremeValue(); }

  /**
   * A proxy variable used to find the quadrature point at
   * which to evaluate the variable. If not provided, defaults to the variable.
   */
  const VariableValue &_inside_sb_region;

  const VariableValue & _proxy_variable;

  /// Extreme value of the value and proxy variable at the same point
  std::pair<Real, Real> _proxy_value;


};
