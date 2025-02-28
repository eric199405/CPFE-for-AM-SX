
#include "ComputeCrystalPlasticityResidualEigenstrain.h"

registerMooseObject("am_sx_superalloyApp", ComputeCrystalPlasticityResidualEigenstrain);

InputParameters
ComputeCrystalPlasticityResidualEigenstrain::validParams()
{
  InputParameters params = ComputeCrystalPlasticityEigenstrainBase::validParams();

  params.addCoupledVar("temperature", "Coupled temperature variable");

  params.addParam<int>("uniform", 0, "-");

  params.addRequiredRangeCheckedParam<std::vector<Real>>(
      "thermal_expansion_coefficients",
      "thermal_expansion_coefficients_size=1 | thermal_expansion_coefficients_size=3 | "
      "thermal_expansion_coefficients_size=6 | thermal_expansion_coefficients_size=9",
      "Vector of values defining the constant second order thermal expansion coefficients, "
      "depending on the degree of anisotropy, this should be of size 1, 3, 6 or 9");

  return params;
}

ComputeCrystalPlasticityResidualEigenstrain::ComputeCrystalPlasticityResidualEigenstrain(
    const InputParameters & parameters)
  : DerivativeMaterialInterface<ComputeCrystalPlasticityEigenstrainBase>(parameters),
    _temperature(coupledValue("temperature")),
    _temperature_old(coupledValueOld("temperature")),

    _uniform(getParam<int>("uniform")),
    
    _ddeformation_gradient_dT(isCoupledConstant("temperature")
                                  ? nullptr
                                  : &declarePropertyDerivative<RankTwoTensor>(
                                        _deformation_gradient_name, coupledName("temperature", 0))),
    _thermal_expansion_coefficients(getParam<std::vector<Real>>("thermal_expansion_coefficients")),
    _lattice_thermal_expansion_coefficients(declareProperty<RankTwoTensor>(
        _eigenstrain_name +
        "_lattice_thermal_expansion_coefficients")) 
{
}

void
ComputeCrystalPlasticityResidualEigenstrain::initQpStatefulProperties()
{
  ComputeCrystalPlasticityEigenstrainBase::initQpStatefulProperties();
  _lattice_thermal_expansion_coefficients[_qp] =
      _thermal_expansion_coefficients.rotated(_crysrot[_qp]);
}

void
ComputeCrystalPlasticityResidualEigenstrain::computeQpDeformationGradient()
{
  Real dtheta = pre_factor() * (_temperature[_qp] - _temperature_old[_qp]) * _substep_dt / _dt;

  RankTwoTensor residual_equivalent_thermal_expansion_increment =
      RankTwoTensor::Identity() - dtheta * _lattice_thermal_expansion_coefficients[_qp];
  _deformation_gradient[_qp] =
      residual_equivalent_thermal_expansion_increment.inverse() * _deformation_gradient_old[_qp];

  if (_ddeformation_gradient_dT)
    (*_ddeformation_gradient_dT)[_qp] =
        _lattice_thermal_expansion_coefficients[_qp] * _deformation_gradient[_qp];
}

double 
ComputeCrystalPlasticityResidualEigenstrain::pre_factor(){

  double rm = pow(_q_point[_qp](0)*_q_point[_qp](0)+_q_point[_qp](1)*_q_point[_qp](1),0.5);
  double r0 = 0;

  double x=_q_point[_qp](0);
  double y=_q_point[_qp](1);

  if (x<0) x=x*(-1);
  if (y<0) y=y*(-1);

  if (x>y) 
  {
      double temp=x;
      x=y;
      y=temp;
  }

    r0 = pow((1 +x*x/y*y ), 0.5);

  if (_q_point[_qp](1)==0 && _q_point[_qp](0)==0) return 1;

  if (_uniform==1) return 1;

  return (std::cos(rm/r0 * M_PI)+1)/2;

}