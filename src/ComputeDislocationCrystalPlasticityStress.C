
#include "ComputeDislocationCrystalPlasticityStress.h"
#include "CrystalPlasticityDislocationDendriteBase.h"
#include "libmesh/utility.h"
#include "Conversion.h"
#include "MooseException.h"

registerMooseObject("SolidMechanicsApp", ComputeDislocationCrystalPlasticityStress);

InputParameters
ComputeDislocationCrystalPlasticityStress::validParams()
{
  InputParameters params = ComputeFiniteStrainElasticStress::validParams();

  params.addClassDescription(
      "Crystal Plasticity base class: handles the Newton iteration over the stress residual and "
      "calculates the Jacobian based on constitutive laws from multiple material classes "
      "that are inherited from CrystalPlasticityDislocationDendriteBase."
	  "The only difference between this class and ComputeMultipleCrystalPlasticityStress "
	  "is that the _models variable here is an array of CrystalPlasticityDislocationDendriteBase "
	  "instead of CrystalPlasticityStressUpdateBase");
  params.addParam<std::string>(
      "base_name",
      "Optional parameter that allows the user to define multiple mechanics material systems on "
      "the same block, i.e. for multiple phases");

  params.addRequiredParam<std::vector<MaterialName>>(
      "crystal_plasticity_models",
      "The material objects to use to calculate crystal plasticity stress and strains.");
  params.addParam<std::vector<MaterialName>>("eigenstrain_names", 
                                             "The material objects to calculate eigenstrains.");
  params.addParam<MooseEnum>("tan_mod_type",
                             MooseEnum("exact none", "none"),
                             "Type of tangent moduli for preconditioner: default elastic");
  params.addParam<Real>("rtol", 1e-6, "Constitutive stress residual relative tolerance");
  params.addParam<Real>("abs_tol", 1e-6, "Constitutive stress residual absolute tolerance");
  params.addParam<unsigned int>("maxiter", 100, "Maximum number of iterations for stress update");
  params.addParam<unsigned int>(
      "maxiter_state_variable", 100, "Maximum number of iterations for state variable update");
  params.addParam<unsigned int>(
      "maximum_substep_iteration", 1, "Maximum number of substep iteration");
  params.addParam<bool>("use_line_search", false, "Use line search in constitutive update");
  params.addParam<Real>("min_line_search_step_size", 0.01, "Minimum line search step size");
  params.addParam<Real>("line_search_tol", 0.5, "Line search bisection method tolerance");
  params.addParam<unsigned int>(
      "line_search_maxiter", 20, "Line search bisection method maximum number of iteration");
  params.addParam<MooseEnum>("line_search_method",
                             MooseEnum("CUT_HALF BISECTION", "CUT_HALF"),
                             "The method used in line search");
  params.addParam<bool>(
      "print_state_variable_convergence_error_messages",
      false,
      "Whether or not to print warning messages from the crystal plasticity specific convergence "
      "checks on the stress measure and general constitutive model quantinties.");
  params.addParam<UserObjectName>("read_initial_Fp",
                                  "The ElementReadPropertyFile "
                                  "GeneralUserObject to read element value "
                                  "of the initial plastic deformation gradient");
  params.addCoupledVar("temperature",303.0,"Temperature");
  params.addParam<Real>("thermal_expansion",0.0,"Thermal expansion coefficient");
  params.addParam<Real>("reference_temperature",303.0,"reference temperature for thermal expansion");
  params.addParam<Real>("dCTE_dT",0.0,"coefficient for the increase of thermal expansion coefficient");
  params.addParam<Real>("melting_temperature_high", 1673.15, "Melting temperature (liquidus) to activate/deactivate liquid thermal expansion.");
  params.addParam<Real>("melting_temperature_low", 1648.15, "Solidus temperature to activate/deactivate liquid thermal expansion.");
  params.addParam<bool>("liquid_thermal_expansion", true, "Liquid has thermal expansion above melting point.");
  return params;
}

ComputeDislocationCrystalPlasticityStress::ComputeDislocationCrystalPlasticityStress(
    const InputParameters & parameters)
  : ComputeFiniteStrainElasticStress(parameters),
    _num_models(getParam<std::vector<MaterialName>>("crystal_plasticity_models").size()),
    _num_eigenstrains(getParam<std::vector<MaterialName>>("eigenstrain_names").size()),
    _base_name(isParamValid("base_name") ? getParam<std::string>("base_name") + "_" : ""),
    _elasticity_tensor(getMaterialPropertyByName<RankFourTensor>(_base_name + "elasticity_tensor")),
    _rtol(getParam<Real>("rtol")),
    _abs_tol(getParam<Real>("abs_tol")),
    _maxiter(getParam<unsigned int>("maxiter")),
    _maxiterg(getParam<unsigned int>("maxiter_state_variable")),
    _tan_mod_type(getParam<MooseEnum>("tan_mod_type").getEnum<TangentModuliType>()),
    _max_substep_iter(getParam<unsigned int>("maximum_substep_iteration")),
    _use_line_search(getParam<bool>("use_line_search")),
    _min_line_search_step_size(getParam<Real>("min_line_search_step_size")),
    _line_search_tolerance(getParam<Real>("line_search_tol")),
    _line_search_max_iterations(getParam<unsigned int>("line_search_maxiter")),
    _line_search_method(getParam<MooseEnum>("line_search_method").getEnum<LineSearchMethod>()),
    _plastic_deformation_gradient(declareProperty<RankTwoTensor>("plastic_deformation_gradient")),
    _plastic_deformation_gradient_old(
        getMaterialPropertyOld<RankTwoTensor>("plastic_deformation_gradient")),
    _eigenstrain_deformation_gradient(
        _num_eigenstrains ? &declareProperty<RankTwoTensor>("eigenstrain_deformation_gradient")
                          : nullptr),
    _eigenstrain_deformation_gradient_old(
        _num_eigenstrains
            ? &getMaterialPropertyOld<RankTwoTensor>("eigenstrain_deformation_gradient")
            : nullptr),
    _deformation_gradient(getMaterialProperty<RankTwoTensor>(_base_name + "deformation_gradient")),
    _deformation_gradient_old(
        getMaterialPropertyOld<RankTwoTensor>(_base_name + "deformation_gradient")),
    _pk2(declareProperty<RankTwoTensor>("second_piola_kirchhoff_stress")),
    _pk2_old(getMaterialPropertyOld<RankTwoTensor>("second_piola_kirchhoff_stress")),
    _total_lagrangian_strain(
        declareProperty<RankTwoTensor>("total_lagrangian_strain")),
    _updated_rotation(declareProperty<RankTwoTensor>("updated_rotation")),
    _crysrot(getMaterialProperty<RankTwoTensor>(
        _base_name + "crysrot")), 
    _print_convergence_message(getParam<bool>("print_state_variable_convergence_error_messages")),				
    _read_initial_Fp(isParamValid("read_initial_Fp")
                               ? &getUserObject<ElementPropertyReadFile>("read_initial_Fp")
                               : nullptr),
							   
	_temperature(coupledValue("temperature")),     
    _thermal_expansion(getParam<Real>("thermal_expansion")),
    _reference_temperature(getParam<Real>("reference_temperature")),
	_dCTE_dT(getParam<Real>("dCTE_dT")),
	_melting_temperature_high(getParam<Real>("melting_temperature_high")),
	_melting_temperature_low(getParam<Real>("melting_temperature_low")),
	_liquid_thermal_expansion(getParam<bool>("liquid_thermal_expansion"))
{
  _convergence_failed = false;
}

void
ComputeDislocationCrystalPlasticityStress::initQpStatefulProperties()
{
  RankTwoTensor initial_Fp;	
	
  if (_read_initial_Fp) { 
	
    for (unsigned int i = 0; i < 3; ++i) {
	  for (unsigned int j = 0; j < 3; ++j) {
        initial_Fp(i,j) = _read_initial_Fp->getData(_current_elem, 3*i+j);
	  }
	}
  
    _plastic_deformation_gradient[_qp] = initial_Fp;
  
  } else { 
  
    _plastic_deformation_gradient[_qp].zero();
    _plastic_deformation_gradient[_qp].addIa(1.0);
  
  }

  if (_num_eigenstrains)
  {
    (*_eigenstrain_deformation_gradient)[_qp].zero();
    (*_eigenstrain_deformation_gradient)[_qp].addIa(1.0);
  }
  else
  {
    _inverse_eigenstrain_deformation_grad.zero();
    _inverse_eigenstrain_deformation_grad.addIa(1.0);
  }

  _pk2[_qp].zero();

  _total_lagrangian_strain[_qp].zero();

  _updated_rotation[_qp].zero();
  _updated_rotation[_qp].addIa(1.0);

  for (unsigned int i = 0; i < _num_models; ++i)
  {
    _models[i]->setQp(_qp);
    _models[i]->initQpStatefulProperties();
  }

  for (unsigned int i = 0; i < _num_eigenstrains; ++i)
  {
    _eigenstrains[i]->setQp(_qp);
    _eigenstrains[i]->initQpStatefulProperties();
  }
}

void
ComputeDislocationCrystalPlasticityStress::initialSetup()
{

  std::vector<MaterialName> model_names =
      getParam<std::vector<MaterialName>>("crystal_plasticity_models");

  for (unsigned int i = 0; i < _num_models; ++i)
  {
    CrystalPlasticityDislocationDendriteBase * model =
        dynamic_cast<CrystalPlasticityDislocationDendriteBase *>(&getMaterialByName(model_names[i]));

    if (model)
    {
      _models.push_back(model);
    }
    else
      mooseError("Model " + model_names[i] +
                 " is not compatible with ComputeDislocationCrystalPlasticityStress");
  }

  std::vector<MaterialName> eigenstrain_names =
      getParam<std::vector<MaterialName>>("eigenstrain_names");

  for (unsigned int i = 0; i < _num_eigenstrains; ++i)
  {
    ComputeCrystalPlasticityEigenstrainBase * eigenstrain =
        dynamic_cast<ComputeCrystalPlasticityEigenstrainBase *>(
            &getMaterialByName(eigenstrain_names[i]));

    if (eigenstrain)
      _eigenstrains.push_back(eigenstrain);
    else
      mooseError("Eigenstrain" + eigenstrain_names[i] +
                 " is not compatible with ComputeDislocationCrystalPlasticityStress");
  }
}

void
ComputeDislocationCrystalPlasticityStress::computeQpStress()
{
  for (unsigned int i = 0; i < _num_models; ++i)
    _models[i]->setQp(_qp);

  for (unsigned int i = 0; i < _num_eigenstrains; ++i)
    _eigenstrains[i]->setQp(_qp);

  updateStress(_stress[_qp], _Jacobian_mult[_qp]);
}

void
ComputeDislocationCrystalPlasticityStress::updateStress(RankTwoTensor & cauchy_stress,
                                                     RankFourTensor & jacobian_mult)
{

  if (isBoundaryMaterial())
    return;

  unsigned int substep_iter = 1;
  unsigned int num_substep = 1;

  _temporary_deformation_gradient_old = _deformation_gradient_old[_qp];
  if (_temporary_deformation_gradient_old.det() == 0)
    _temporary_deformation_gradient_old.addIa(1.0);

  _delta_deformation_gradient = _deformation_gradient[_qp] - _temporary_deformation_gradient_old;

  for (unsigned int i = 0; i < _num_models; ++i)
    _models[i]->calculateFlowDirection(_crysrot[_qp]);

  do
  {
    _convergence_failed = false;
    preSolveQp();

    _substep_dt = _dt / num_substep;
    for (unsigned int i = 0; i < _num_models; ++i)
    
      _models[i]->setSubstepDt(_substep_dt);
      
    if (_num_eigenstrains)
      calculateEigenstrainDeformationGrad();

    for (unsigned int istep = 0; istep < num_substep; ++istep)
    {
      _temporary_deformation_gradient =
          (static_cast<Real>(istep) + 1) / num_substep * _delta_deformation_gradient;
      _temporary_deformation_gradient += _temporary_deformation_gradient_old;

      solveQp();

      if (_convergence_failed)
      {
        if (_print_convergence_message)
          mooseWarning(
              "The crystal plasticity constitutive model has failed to converge. Increasing "
              "the number of substeps.");

        substep_iter++;
        num_substep *= 2;
        break;
      }
    }

    if (substep_iter > _max_substep_iter && _convergence_failed)
      mooseException("ComputeDislocationCrystalPlasticityStress: Constitutive failure");
  } while (_convergence_failed);

  postSolveQp(cauchy_stress, jacobian_mult);
}

void
ComputeDislocationCrystalPlasticityStress::preSolveQp()
{
  for (unsigned int i = 0; i < _num_models; ++i)
    _models[i]->setInitialConstitutiveVariableValues();

  _pk2[_qp] = _pk2_old[_qp];
  _inverse_plastic_deformation_grad_old = _plastic_deformation_gradient_old[_qp].inverse();
}

void
ComputeDislocationCrystalPlasticityStress::solveQp()
{
  for (unsigned int i = 0; i < _num_models; ++i)
  {
    _models[i]->setSubstepConstitutiveVariableValues();
    _models[i]->calculateSlipResistance();
  }

  _inverse_plastic_deformation_grad = _inverse_plastic_deformation_grad_old;

  solveStateVariables();
  if (_convergence_failed)
    return;

  for (unsigned int i = 0; i < _num_models; ++i)
    _models[i]->updateSubstepConstitutiveVariableValues();
    _inverse_plastic_deformation_grad_old = _inverse_plastic_deformation_grad;
}

void
ComputeDislocationCrystalPlasticityStress::postSolveQp(RankTwoTensor & cauchy_stress,
                                                    RankFourTensor & jacobian_mult)
{
  cauchy_stress = _elastic_deformation_gradient * _pk2[_qp] *
                  _elastic_deformation_gradient.transpose() / _elastic_deformation_gradient.det();

  calcTangentModuli(jacobian_mult);

  _total_lagrangian_strain[_qp] =
      _deformation_gradient[_qp].transpose() * _deformation_gradient[_qp] -
      RankTwoTensor::Identity();
  _total_lagrangian_strain[_qp] = _total_lagrangian_strain[_qp] * 0.5;


  RankTwoTensor rot;
  _elastic_deformation_gradient.getRUDecompositionRotation(rot);
  _updated_rotation[_qp] = rot * _crysrot[_qp];
}

void
ComputeDislocationCrystalPlasticityStress::solveStateVariables()
{
  unsigned int iteration;
  bool iter_flag = true;

  iteration = 0;

  do
  {
    solveStress();
    if (_convergence_failed)
      return;

    _plastic_deformation_gradient[_qp] =
        _inverse_plastic_deformation_grad.inverse();

    for (unsigned int i = 0; i < _num_models; ++i)
      _models[i]->cacheStateVariablesBeforeUpdate();

    for (unsigned int i = 0; i < _num_models; ++i)
      _models[i]->calculateStateVariableEvolutionRateComponent();

    for (unsigned int i = 0; i < _num_models; ++i)
      if (!_models[i]->updateStateVariables())
        _convergence_failed = true;

    for (unsigned int i = 0; i < _num_models; ++i)
      _models[i]->calculateSlipResistance();

    if (_convergence_failed)
      return;

    for (unsigned int i = 0; i < _num_models; ++i)
    {
      if (!_models[i]->areConstitutiveStateVariablesConverged())
      {
        iter_flag = true;
        break;
      }
      else
        iter_flag = false;
    }

    if (iter_flag)
    {
      if (_print_convergence_message)
        mooseWarning("ComputeDislocationCrystalPlasticityStress: State variables (or the system "
                     "resistance) did not converge at element ",
                     _current_elem->id(),
                     " and qp ",
                     _qp,
                     "\n");
    }
    iteration++;
  } while (iter_flag && iteration < _maxiterg);

  if (iteration == _maxiterg)
  {
    if (_print_convergence_message)
      mooseWarning(
          "ComputeDislocationCrystalPlasticityStress: Hardness Integration error. Reached the "
          "maximum number of iterations to solve for the state variables at element ",
          _current_elem->id(),
          " and qp ",
          _qp,
          "\n");

    _convergence_failed = true;
  }
}

void
ComputeDislocationCrystalPlasticityStress::solveStress()
{
  unsigned int iteration = 0;
  RankTwoTensor dpk2;
  Real rnorm, rnorm0, rnorm_prev;

  calculateResidualAndJacobian();
  if (_convergence_failed)
  {
    if (_print_convergence_message)
      mooseWarning("ComputeDislocationCrystalPlasticityStress: the slip increment exceeds tolerance "
                   "at element ",
                   _current_elem->id(),
                   " and Gauss point ",
                   _qp);

    return;
  }

  rnorm = _residual_tensor.L2norm();
  rnorm0 = rnorm;

  while (rnorm > _rtol * rnorm0 && rnorm > _abs_tol && iteration < _maxiter)
  {
    dpk2 = -_jacobian.invSymm() * _residual_tensor;
    _pk2[_qp] = _pk2[_qp] + dpk2;

    calculateResidualAndJacobian();

    if (_convergence_failed)
    {
      if (_print_convergence_message)
        mooseWarning("ComputeDislocationCrystalPlasticityStress: the slip increment exceeds tolerance "
                     "at element ",
                     _current_elem->id(),
                     " and Gauss point ",
                     _qp);

      return;
    }

    rnorm_prev = rnorm;
    rnorm = _residual_tensor.L2norm();

    if (_use_line_search && rnorm > rnorm_prev && !lineSearchUpdate(rnorm_prev, dpk2))
    {
      if (_print_convergence_message)
        mooseWarning("ComputeDislocationCrystalPlasticityStress: Failed with line search");

      _convergence_failed = true;
      return;
    }

    if (_use_line_search)
      rnorm = _residual_tensor.L2norm();

    iteration++;
  }

  if (iteration >= _maxiter)
  {
    if (_print_convergence_message)
      mooseWarning("ComputeDislocationCrystalPlasticityStress: Stress Integration error rmax = ",
                   rnorm,
                   " and the tolerance is ",
                   _rtol * rnorm0,
                   " when the rnorm0 value is ",
                   rnorm0,
                   " for element ",
                   _current_elem->id(),
                   " and qp ",
                   _qp);

    _convergence_failed = true;
  }
}

void
ComputeDislocationCrystalPlasticityStress::calculateResidualAndJacobian()
{
  calculateResidual();
  if (_convergence_failed)
    return;
  calculateJacobian();
}

void
ComputeDislocationCrystalPlasticityStress::calculateResidual()
{
  RankTwoTensor ce, elastic_strain, ce_pk2, equivalent_slip_increment_per_model,
      equivalent_slip_increment, pk2_new;
	  
  RankTwoTensor thermal_eigenstrain;

  equivalent_slip_increment.zero();

  for (unsigned int i = 0; i < _num_models; ++i)
  {
    equivalent_slip_increment_per_model.zero();

    _models[i]->calculateShearStress(
        _pk2[_qp], _inverse_eigenstrain_deformation_grad, _num_eigenstrains);

    _convergence_failed = !_models[i]->calculateSlipRate();

    if (_convergence_failed)
      return;

    _models[i]->calculateEquivalentSlipIncrement(equivalent_slip_increment_per_model);
    equivalent_slip_increment += equivalent_slip_increment_per_model;
  }

  RankTwoTensor residual_equivalent_slip_increment =
      RankTwoTensor::Identity() - equivalent_slip_increment;
  _inverse_plastic_deformation_grad =
      _inverse_plastic_deformation_grad_old * residual_equivalent_slip_increment;

  _elastic_deformation_gradient = _temporary_deformation_gradient *
                                  _inverse_eigenstrain_deformation_grad *
                                  _inverse_plastic_deformation_grad;

  ce = _elastic_deformation_gradient.transpose() * _elastic_deformation_gradient;

  elastic_strain = ce - RankTwoTensor::Identity();
  elastic_strain *= 0.5;
  
  calculateThermalEigenstrain(thermal_eigenstrain);

  pk2_new = _elasticity_tensor[_qp] * (elastic_strain - thermal_eigenstrain);
  _residual_tensor = _pk2[_qp] - pk2_new;
}

void
ComputeDislocationCrystalPlasticityStress::calculateJacobian()
{
  RankFourTensor dfedfpinv, deedfe, dfpinvdpk2, dfpinvdpk2_per_model;

  RankTwoTensor ffeiginv = _temporary_deformation_gradient * _inverse_eigenstrain_deformation_grad;

  for (unsigned int i = 0; i < LIBMESH_DIM; ++i)
    for (unsigned int j = 0; j < LIBMESH_DIM; ++j)
      for (unsigned int k = 0; k < LIBMESH_DIM; ++k)
        dfedfpinv(i, j, k, j) = ffeiginv(i, k);

  for (unsigned int i = 0; i < LIBMESH_DIM; ++i)
    for (unsigned int j = 0; j < LIBMESH_DIM; ++j)
      for (unsigned int k = 0; k < LIBMESH_DIM; ++k)
      {
        deedfe(i, j, k, i) = deedfe(i, j, k, i) + _elastic_deformation_gradient(k, j) * 0.5;
        deedfe(i, j, k, j) = deedfe(i, j, k, j) + _elastic_deformation_gradient(k, i) * 0.5;
      }

  for (unsigned int i = 0; i < _num_models; ++i)
  {
    _models[i]->calculateTotalPlasticDeformationGradientDerivative(
        dfpinvdpk2_per_model,
        _inverse_plastic_deformation_grad_old,
        _inverse_eigenstrain_deformation_grad,
        _num_eigenstrains);
    dfpinvdpk2 += dfpinvdpk2_per_model;
  }

  _jacobian =
      RankFourTensor::IdentityFour() - (_elasticity_tensor[_qp] * deedfe * dfedfpinv * dfpinvdpk2);
}

void
ComputeDislocationCrystalPlasticityStress::calcTangentModuli(RankFourTensor & jacobian_mult)
{
  switch (_tan_mod_type)
  {
    case TangentModuliType::EXACT:
      elastoPlasticTangentModuli(jacobian_mult);
      break;
    default:
      elasticTangentModuli(jacobian_mult);
  }
}

void
ComputeDislocationCrystalPlasticityStress::elastoPlasticTangentModuli(RankFourTensor & jacobian_mult)
{
  RankFourTensor tan_mod;
  RankTwoTensor pk2fet, fepk2, feiginvfpinv;
  RankFourTensor deedfe, dsigdpk2dfe, dfedf;

  for (const auto i : make_range(Moose::dim))
    for (const auto j : make_range(Moose::dim))
      for (const auto k : make_range(Moose::dim))
      {
        deedfe(i, j, k, i) = deedfe(i, j, k, i) + _elastic_deformation_gradient(k, j) * 0.5;
        deedfe(i, j, k, j) = deedfe(i, j, k, j) + _elastic_deformation_gradient(k, i) * 0.5;
      }

  usingTensorIndices(i_, j_, k_, l_);
  dsigdpk2dfe = _elastic_deformation_gradient.times<i_, k_, j_, l_>(_elastic_deformation_gradient) *
                _elasticity_tensor[_qp] * deedfe;

  pk2fet = _pk2[_qp] * _elastic_deformation_gradient.transpose();
  fepk2 = _elastic_deformation_gradient * _pk2[_qp];

  for (const auto i : make_range(Moose::dim))
    for (const auto j : make_range(Moose::dim))
      for (const auto l : make_range(Moose::dim))
      {
        tan_mod(i, j, i, l) += pk2fet(l, j);
        tan_mod(i, j, j, l) += fepk2(i, l);
      }

  tan_mod += dsigdpk2dfe;

  const auto je = _elastic_deformation_gradient.det();
  if (je > 0.0)
    tan_mod /= je;

  feiginvfpinv = _inverse_eigenstrain_deformation_grad * _inverse_plastic_deformation_grad;
  for (const auto i : make_range(Moose::dim))
    for (const auto j : make_range(Moose::dim))
      for (const auto l : make_range(Moose::dim))
        dfedf(i, j, i, l) = feiginvfpinv(l, j);

  jacobian_mult = tan_mod * dfedf;
}

void
ComputeDislocationCrystalPlasticityStress::elasticTangentModuli(RankFourTensor & jacobian_mult)
{
  jacobian_mult = _elasticity_tensor[_qp];
}

bool
ComputeDislocationCrystalPlasticityStress::lineSearchUpdate(const Real & rnorm_prev,
                                                         const RankTwoTensor & dpk2)
{
  if (_line_search_method == LineSearchMethod::CutHalf)
  {
    Real rnorm;
    Real step = 1.0;

    do
    {
      _pk2[_qp] = _pk2[_qp] - step * dpk2;
      step /= 2.0;
      _pk2[_qp] = _pk2[_qp] + step * dpk2;

      calculateResidual();
      rnorm = _residual_tensor.L2norm();
    } while (rnorm > rnorm_prev && step > _min_line_search_step_size);

    return (rnorm <= rnorm_prev || step > _min_line_search_step_size);
  }
  else if (_line_search_method == LineSearchMethod::Bisection)
  {
    unsigned int count = 0;
    Real step_a = 0.0;
    Real step_b = 1.0;
    Real step = 1.0;
    Real s_m = 1000.0;
    Real rnorm = 1000.0;

    calculateResidual();
    auto s_b = _residual_tensor.doubleContraction(dpk2);
    const auto rnorm1 = _residual_tensor.L2norm();
    _pk2[_qp] = _pk2[_qp] - dpk2;
    calculateResidual();
    auto s_a = _residual_tensor.doubleContraction(dpk2);
    const auto rnorm0 = _residual_tensor.L2norm();
    _pk2[_qp] = _pk2[_qp] + dpk2;

    if ((rnorm1 / rnorm0) < _line_search_tolerance || s_a * s_b > 0)
    {
      calculateResidual();
      return true;
    }

    while ((rnorm / rnorm0) > _line_search_tolerance && count < _line_search_max_iterations)
    {
      _pk2[_qp] = _pk2[_qp] - step * dpk2;
      step = 0.5 * (step_b + step_a);
      _pk2[_qp] = _pk2[_qp] + step * dpk2;
      calculateResidual();
      s_m = _residual_tensor.doubleContraction(dpk2);
      rnorm = _residual_tensor.L2norm();

      if (s_m * s_a < 0.0)
      {
        step_b = step;
        s_b = s_m;
      }
      if (s_m * s_b < 0.0)
      {
        step_a = step;
        s_a = s_m;
      }
      count++;
    }

    return ((rnorm / rnorm0) < _line_search_tolerance && count < _line_search_max_iterations);
  }
  else
    mooseError("Line search method is not provided.");
}

void
ComputeDislocationCrystalPlasticityStress::calculateEigenstrainDeformationGrad()
{
  _inverse_eigenstrain_deformation_grad.zero();
  _inverse_eigenstrain_deformation_grad.addIa(1.0);

  for (unsigned int i = 0; i < _num_eigenstrains; ++i)
  {
    _eigenstrains[i]->setSubstepDt(_substep_dt);
    _eigenstrains[i]->computeQpProperties();
    _inverse_eigenstrain_deformation_grad *= _eigenstrains[i]->getDeformationGradientInverse();
  }
  (*_eigenstrain_deformation_gradient)[_qp] = _inverse_eigenstrain_deformation_grad.inverse();
}


void
ComputeDislocationCrystalPlasticityStress::calculateThermalEigenstrain(RankTwoTensor & thermal_eigenstrain)
{

  RankTwoTensor max_thermal_eigenstrain;
  
  Real deltaT;
  
  max_thermal_eigenstrain = 0.5 * (
                                    std::exp(
                                              (1.0 / 3.0) * _dCTE_dT * ( _temperature[_qp] - _reference_temperature ) * ( _temperature[_qp] - _reference_temperature )
                                            + (2.0 / 3.0) * _thermal_expansion * ( _temperature[_qp] - _reference_temperature ) 
                                            ) 
								    - 1.0
								  ) * RankTwoTensor::Identity();

  if (_liquid_thermal_expansion) { 
	  
    thermal_eigenstrain = max_thermal_eigenstrain;
    
  }	else { 
	  
    if (_temperature[_qp] > _melting_temperature_high) { 
	
	  thermal_eigenstrain.zero();
	
	} else if (_temperature[_qp] > _melting_temperature_low) { 
		
      deltaT = _melting_temperature_high - _melting_temperature_low;
      
      thermal_eigenstrain = (_melting_temperature_high - _temperature[_qp]) * max_thermal_eigenstrain / deltaT;
		
	} else { 

      thermal_eigenstrain = max_thermal_eigenstrain;

	} 
  }
}


