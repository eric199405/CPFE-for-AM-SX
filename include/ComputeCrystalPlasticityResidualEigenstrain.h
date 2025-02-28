

#pragma once

#include "ComputeCrystalPlasticityEigenstrainBase.h"
#include "DerivativeMaterialInterface.h"

class ComputeCrystalPlasticityResidualEigenstrain
  : public DerivativeMaterialInterface<ComputeCrystalPlasticityEigenstrainBase>
{
public:
  static InputParameters validParams();

  ComputeCrystalPlasticityResidualEigenstrain(const InputParameters & parameters);

  /// We need to set initial values for lattice thermal expansion coefficients
  virtual void initQpStatefulProperties() override;

protected:
  ///Compute the deformation gradient due to thermal expansion
  virtual void computeQpDeformationGradient() override;

  ///Temperature variable value
  const VariableValue & _temperature;
  const VariableValue & _temperature_old;

  const int _uniform;

  ///Stores the derivative of the deforamtion gradient w.r.t temperature
  MaterialProperty<RankTwoTensor> * _ddeformation_gradient_dT;

  ///The thermal expansion coefficient that is definted in the cartesian coordinate
  const RankTwoTensor _thermal_expansion_coefficients;

  ///Stores the thermal expansion coefficient w.r.t. the lattice symmetry axis
  MaterialProperty<RankTwoTensor> & _lattice_thermal_expansion_coefficients;

  double pre_factor();

};
