// Zixu Guo
// National University of Singapore
// Yilun Xu
// Imperial College London, IHPC A*STAR
// 01 MAY 2023

#pragma once

#include "DirichletBCBase.h"

class Function;

/**
 * Implements a Dirichlet BC where u=_coupled_var on the boundary.
 */
class CoupledVarDirichletBC : public DirichletBCBase
{
public:
  static InputParameters validParams();

  CoupledVarDirichletBC(const InputParameters & parameters);

protected:
  virtual Real computeQpValue() override;
  // virtual Real computeQpOffDiagJacobian(unsigned int jvar) override;

  /// Variable providing the value u on the boundary.
  const VariableValue & _coupled_var;

  /// The identifying number of the coupled variable
  const unsigned int _coupled_num;

  /// A coefficient that is multiplied with the residual contribution
  const Real _coef;

  /// Scale factor
  const VariableValue & _scale_factor;

  const Function & _func;
};