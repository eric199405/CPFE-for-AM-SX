// Zixu Guo
// National University of Singapore
// Yilun Xu
// Imperial College London, IHPC A*STAR
// 01 MAY 2023

#include "CrystalPlasticityDislocationDendrite.h"
#include "libmesh/int_range.h"
#include <cmath>
#include "Function.h"

registerMooseObject("SolidMechanicsApp", CrystalPlasticityDislocationDendrite);

InputParameters 
CrystalPlasticityDislocationDendrite::validParams()
{
  InputParameters params = CrystalPlasticityDislocationDendriteBase::validParams();
  params.addClassDescription("Dislocation based model for crystal plasticity "
                             "using the stress update code. "
                             "Includes slip, creep and backstress. ");
  params.addParam<Real>("ao", 0.001, "slip rate coefficient for gamma phase");
  params.addParam<Real>("xm", 0.1, "strain rate sensitivity for gamma phase");  
  params.addParam<Real>("ao2", 0, "slip rate coefficient");
  params.addParam<Real>("xm2", 0.1, "strain rate sensitivity for gamma prime phase");  
  params.addParam<MaterialPropertyName>("xm_matprop",
    "Optional xm material property for exponent for slip rate. ");
  params.addParam<Real>("creep_ao", 0.0, "creep rate coefficient");
  params.addParam<Real>("creep_xm", 0.1, "exponent for creep rate");
  params.addParam<FunctionName>("creep_ao_function",
    "Optional function for creep prefactor. If provided, the creep prefactor can be set as a function of time. "
    "This is useful for an initial plastic deformation followed by creep load. ");
  params.addParam<bool>("cap_slip_increment", false, "Cap the absolute value of the slip increment "
                                                     "in one time step to _slip_incr_tol. ");
  params.addParam<Real>("burgers_vector_mag",0.000255,"Magnitude of the Burgers vector");
  params.addParam<Real>("shear_modulus",35000,"Shear modulus in Taylor hardening law G");
  params.addParam<Real>("alpha_0",0.3,"Prefactor of Taylor hardening law, alpha");
  params.addParam<Real>("r", 1.4, "Latent hardening coefficient");
  params.addParam<Real>("tau_c_0", 0.112, "Peierls stress");
  params.addParam<Real>("k_0",100.0,"Coefficient K in SSD evolution, representing accumulation rate");
  params.addParam<Real>("y_c",0.0026,"Critical annihilation diameter");
  params.addParam<Real>("h",0.0,"Direct hardening coefficient for backstress");
  params.addParam<Real>("h_D",0.0,"Dynamic recovery coefficient for backstress");
  params.addParam<Real>("rho_tol",1.0,"Tolerance on dislocation density update");
  params.addParam<Real>("scale", 7.5, "physcial length per unit length in FEM model unit: um");
  params.addParam<Real>("gamma_APB", 0.055, "anti-phase boundary energy / J");
  params.addParam<Real>("G_shear", 35000, "shear modulus for the calculation of Orowan stress / MPa");
  params.addParam<Real>("k_solute", 1, "relative intensities of TGMs, type 4");
  params.addParam<Real>("k_HL", 1, "relative intensities of TGMs, type 2");
  params.addParam<Real>("k_w", 1, "relative intensities of TGMs, type 3");
  params.addParam<int>("inhomo_or", 1, "enable (1) and disable (0) TGMs, type 3");
  params.addParam<int>("inhomo_shear", 1, "enable (1) and disable (0) TGMs, type 2");
  params.addParam<int>("inhomo_solute", 1, "enable (1) and disable (0) TGMs, type 4");
  params.addParam<int>("with_GND", 1, "enable (1) and disable (0) GNDs during the deformation");
  params.addParam<int>("with_residual_dis", 1, "enable (1) and disable (0) residual SSD density");
  params.addParam<Real>("z_length", 6, "total length of the whole RVE");
  params.addParam<Real>("sb_evo_rate", 0.0015, "slip band propagation rate");
  params.addParam<Real>("initial_SB_width", 0.4, "initial slip band width");
  params.addParam<Real>("Wcr", 67.5, "critical dissipation energy");
  params.addParam<Real>("Dcr", 0.4, "threshold damage for the failure");
  params.addRequiredParam<PostprocessorName>("D1_old","--");
  params.addRequiredParam<PostprocessorName>("D2_old","--");
  params.addRequiredParam<PostprocessorName>("sb_initiation_old","--");
  params.addRequiredParam<PostprocessorName>("stress_zz_old","--");
  params.addRequiredParam<PostprocessorName>("strain_zz_old","--");
  params.addRequiredParam<PostprocessorName>("max_crss_shear","--");
  params.addRequiredParam<PostprocessorName>("min_crss_shear","--");
  params.addParam<Real>("initial_shear", 140, "used in the simulation of slip band nucleation");
  params.addParam<Real>("end_strain", 0.15, "used in the simulation of slip band propagation");
  params.addParam<Real>("init_rho_ssd",1.0,"Initial dislocation density");
  params.addParam<Real>("init_rho_gnd_edge",0.0,"Initial dislocation density");
  params.addParam<Real>("init_rho_gnd_screw",0.0,"Initial dislocation density");
  params.addParam<MaterialPropertyName>(
      "total_twin_volume_fraction",
      "Total twin volume fraction, if twinning is considered in the simulation");
  params.addParam<UserObjectName>("read_conc_ele", "inital distributed solute concentrations");
  params.addParam<UserObjectName>("read_micro_morph", "inital distributed gamma / gamma prime morphology");
  params.addCoupledVar("dslip_increment_dedge",0.0,"Directional derivative of the slip rate along the edge motion direction.");
  params.addCoupledVar("dslip_increment_dscrew",0.0,"Directional derivative of the slip rate along the screw motion direction.");
  params.addCoupledVar("ini_stress",0.0,"-");
  params.addCoupledVar("ini_strain",0.0,"-");
  return params;
}

CrystalPlasticityDislocationDendrite::CrystalPlasticityDislocationDendrite(
    const InputParameters & parameters)
  : CrystalPlasticityDislocationDendriteBase(parameters),
    _ao(getParam<Real>("ao")),
    _xm(getParam<Real>("xm")),
    _ao2(getParam<Real>("ao2")),
    _xm2(getParam<Real>("xm2")),
    _include_xm_matprop(parameters.isParamValid("xm_matprop")),
    _xm_matprop(_include_xm_matprop
                ? &getMaterialProperty<Real>("xm_matprop")
                : nullptr),
    _creep_ao(getParam<Real>("creep_ao")),
    _creep_xm(getParam<Real>("creep_xm")),
    _creep_ao_function(this->isParamValid("creep_ao_function")
                       ? &this->getFunction("creep_ao_function")
                       : NULL),
    _cap_slip_increment(getParam<bool>("cap_slip_increment")),
    _burgers_vector_mag(getParam<Real>("burgers_vector_mag")),
    _shear_modulus(getParam<Real>("shear_modulus")),
    _alpha_0(getParam<Real>("alpha_0")),
    _r(getParam<Real>("r")),
    _tau_c_0(getParam<Real>("tau_c_0")),
    _k_0(getParam<Real>("k_0")),
    _y_c(getParam<Real>("y_c")),
    _h(getParam<Real>("h")),
    _h_D(getParam<Real>("h_D")),
    _init_rho_ssd(getParam<Real>("init_rho_ssd")),
    _init_rho_gnd_edge(getParam<Real>("init_rho_gnd_edge")),
    _init_rho_gnd_screw(getParam<Real>("init_rho_gnd_screw")),
    _rho_tol(getParam<Real>("rho_tol")),
    _temp_gss(_number_slip_systems, 0.0),
    _scale(getParam<Real>("scale")),
    _gamma_APB(getParam<Real>("gamma_APB")),
	  _G_shear(getParam<Real>("G_shear")),
    _k_solute(getParam<Real>("k_solute")),
    _k_HL(getParam<Real>("k_HL")),
    _k_w(getParam<Real>("k_w")),
    _inhomo_or(getParam<int>("inhomo_or")),
    _inhomo_shear(getParam<int>("inhomo_shear")),
    _inhomo_solute(getParam<int>("inhomo_solute")),
    _with_GND(getParam<int>("with_GND")),
    _with_residual_dis(getParam<int>("with_residual_dis")),
    _z_length(getParam<Real>("z_length")),
    _sb_evo_rate(getParam<Real>("sb_evo_rate")),
    _initial_SB_width(getParam<Real>("initial_SB_width")),
    _Wcr(getParam<Real>("Wcr")),
    _Dcr(getParam<Real>("Dcr")),
    _D1_old(getPostprocessorValue("D1_old")),
    _D2_old(getPostprocessorValue("D2_old")),
    _sb_initiation_old(getPostprocessorValue("sb_initiation_old")),
    _stress_zz_old(getPostprocessorValue("stress_zz_old")),
    _strain_zz_old(getPostprocessorValue("strain_zz_old")),
    _max_crss_shear(getPostprocessorValue("max_crss_shear")),
    _min_crss_shear(getPostprocessorValue("min_crss_shear")),
    _initial_shear(getParam<Real>("initial_shear")),
    _end_strain(getParam<Real>("end_strain")),
    _rho_ssd(declareProperty<std::vector<Real>>("rho_ssd")),
    _rho_ssd_old(getMaterialPropertyOld<std::vector<Real>>("rho_ssd")),
    _rho_gnd_edge(declareProperty<std::vector<Real>>("rho_gnd_edge")),
   	_rho_gnd_edge_old(getMaterialPropertyOld<std::vector<Real>>("rho_gnd_edge")),
  	_rho_gnd_screw(declareProperty<std::vector<Real>>("rho_gnd_screw")),
    _rho_gnd_screw_old(getMaterialPropertyOld<std::vector<Real>>("rho_gnd_screw")),
    _backstress(declareProperty<std::vector<Real>>("backstress")),
    _backstress_old(getMaterialPropertyOld<std::vector<Real>>("backstress")),
    _rho_ssd_increment(_number_slip_systems, 0.0),
    _rho_gnd_edge_increment(_number_slip_systems, 0.0),
    _rho_gnd_screw_increment(_number_slip_systems, 0.0),
    _backstress_increment(_number_slip_systems, 0.0),
    _previous_substep_rho_ssd(_number_slip_systems, 0.0),
    _previous_substep_rho_gnd_edge(_number_slip_systems, 0.0),
    _previous_substep_rho_gnd_screw(_number_slip_systems, 0.0),
    _previous_substep_backstress(_number_slip_systems, 0.0),
    _damage_increment(_number_slip_systems, 0.0),
    _previous_substep_damage(_number_slip_systems, 0.0),
    _rho_ssd_before_update(_number_slip_systems, 0.0),
    _rho_gnd_edge_before_update(_number_slip_systems, 0.0),
    _rho_gnd_screw_before_update(_number_slip_systems, 0.0),  	
    _backstress_before_update(_number_slip_systems, 0.0),
    _damage_before_update(_number_slip_systems, 0.0),
    _include_twinning_in_Lp(parameters.isParamValid("total_twin_volume_fraction")),
    _twin_volume_fraction_total(_include_twinning_in_Lp
                                    ? &getMaterialPropertyOld<Real>("total_twin_volume_fraction")
                                    : nullptr),	
    _read_conc_ele(isParamValid("read_conc_ele")
                               ? &getUserObject<PropertyReadFile>("read_conc_ele")
                               : nullptr),
    _read_micro_morph(isParamValid("read_micro_morph")
                               ? &getUserObject<PropertyReadFile>("read_micro_morph")
                               : nullptr),									
    _dslip_increment_dedge(coupledArrayValue("dslip_increment_dedge")), 
    _dslip_increment_dscrew(coupledArrayValue("dslip_increment_dscrew")),
    _edge_slip_direction(declareProperty<std::vector<Real>>("edge_slip_direction")),
    _screw_slip_direction(declareProperty<std::vector<Real>>("screw_slip_direction")),
    _acc_slip(declareProperty<std::vector<Real>>(_base_name + "acc_slip")),
    _acc_slip_old(getMaterialPropertyOld<std::vector<Real>>(_base_name + "acc_slip")),
    _crss_or(declareProperty<std::vector<Real>>(_base_name + "crss_or")),
    _crss_gp_shear(declareProperty<std::vector<Real>>(_base_name + "crss_gp_shear")),
    _residual_ssd(declareProperty<std::vector<Real>>(_base_name + "residual_ssd")),
    _crss_solute(declareProperty<std::vector<Real>>(_base_name + "crss_solute")),
    _conc_ele(declareProperty<std::vector<Real>>(_base_name + "conc_ele")),
    _micro_morph(declareProperty<std::vector<Real>>(_base_name + "micro_morph")),
    _gp_H(declareProperty<Real>(_base_name + "gp_H")),
    _gp_L(declareProperty<Real>(_base_name + "gp_L")),
    _g_wH(declareProperty<Real>(_base_name + "g_wH")),
    _g_wL(declareProperty<Real>(_base_name + "g_wL")),
    _gp_fr(declareProperty<Real>(_base_name + "gp_fr")),
    _crysrot(getMaterialProperty<RankTwoTensor>(_base_name + "crysrot")),
    _residual_eigenstrain(getMaterialProperty<RankTwoTensor>("residual_eigenstrain")),
    _110_yy(declareProperty<Real>(_base_name + "110_yy")),
    _110_xy(declareProperty<Real>(_base_name + "110_xy")),
    _DD(declareProperty<Real>(_base_name + "DD")),
    _SB_initiation(declareProperty<Real>(_base_name + "SB_initiation")),
    _SB_initiation_stress(declareProperty<Real>("SB_initiation_stress")),
    _SB_initiation_strain(declareProperty<Real>("SB_initiation_strain")),
    _ave_damage(declareProperty<Real>("ave_damage")),
    _epsilon_p(declareProperty<RankTwoTensor>("epsilon_p")),
    _epsilon_e(declareProperty<RankTwoTensor>("epsilon_e")),
    _epsilon_total(declareProperty<RankTwoTensor>("epsilon_total")),
    _inside_sb_region(declareProperty<Real>( "inside_sb_region")),
    _inside_sb_region_old(getMaterialPropertyOld<Real>( "inside_sb_region")),
    _slip_resistance_gamma(declareProperty<std::vector<Real>>( "slip_resistance_gamma")),
    _slip_resistance_gamma_prime(declareProperty<std::vector<Real>>("slip_resistance_gamma_prime")),
    _ratio(declareProperty<std::vector<Real>>("ratio")),
    _ini_e_p(declareProperty<RankTwoTensor>(_base_name + "ini_e_p")),
    _ini_e_total(declareProperty<RankTwoTensor>(_base_name + "ini_e_total")),
    _plastic_deformation_gradient(getMaterialProperty<RankTwoTensor>(_base_name + "plastic_deformation_gradient")),
    _deformation_gradient(getMaterialProperty<RankTwoTensor>(_base_name + "deformation_gradient")),
    _state_variable(coupledValue("ini_stress")),
    _state_variable2(coupledValue("ini_strain")),
    _damage(declareProperty<std::vector<Real>>(_base_name + "damage")),
    _damage_old(getMaterialPropertyOld<std::vector<Real>>(_base_name + "damage")),
    _tau_old(getMaterialPropertyOld<std::vector<Real>>(_base_name + "applied_shear_stress"))
{
}

int activited_slip_sys = -1;
void
CrystalPlasticityDislocationDendrite::initQpStatefulProperties()
{
  // Slip resistance is resized here
  CrystalPlasticityDislocationDendriteBase::initQpStatefulProperties();
  
  Real taylor_hardening;

  _rho_ssd[_qp].resize(_number_slip_systems);
  _rho_gnd_edge[_qp].resize(_number_slip_systems);
  _rho_gnd_screw[_qp].resize(_number_slip_systems);
  _backstress[_qp].resize(_number_slip_systems);
  _conc_ele[_qp].resize(5); // Al/Co/Cr/Mo/Ti, in order
  _micro_morph[_qp].resize(5);

  for (const auto i : make_range(_number_slip_systems))
  {
    _rho_ssd[_qp][i] = _init_rho_ssd;
    _rho_gnd_edge[_qp][i] = _init_rho_gnd_edge;
    _rho_gnd_screw[_qp][i] = _init_rho_gnd_screw;
	  _backstress[_qp][i] = 0.0;	
  }

  for (const auto i : make_range(_number_slip_systems))
  {
    _slip_resistance[_qp][i] = _tau_c_0;
    taylor_hardening = 0.0;
    for (const auto j : make_range(_number_slip_systems))
    {
      unsigned int iplane, jplane;
      iplane = i / 3;
      jplane = j / 3;
      if (iplane == jplane) { 
	    taylor_hardening += (_rho_ssd[_qp][j] 
		          + std::abs(_rho_gnd_edge[_qp][j])
				      + std::abs(_rho_gnd_screw[_qp][j])); } 
      else { 
        taylor_hardening += (_r * (_rho_ssd[_qp][j] 
                + std::abs(_rho_gnd_edge[_qp][j])
                + std::abs(_rho_gnd_screw[_qp][j])));	  
	  }
    }
	  _slip_resistance[_qp][i] += (_alpha_0 * _shear_modulus * _burgers_vector_mag
	                          * std::sqrt(taylor_hardening));
  }

  for (const auto i : make_range(_number_slip_systems))
  {
    _slip_increment[_qp][i] = 0.0;
  }

  // Initialize vectors size here because they are used by AuxKernels
  // that are called just after initialization  
  _edge_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);
  _screw_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);
  _acc_slip[_qp].resize(_number_slip_systems);
  _crss_or[_qp].resize(_number_slip_systems);
  _crss_gp_shear[_qp].resize(_number_slip_systems);
  _residual_ssd[_qp].resize(_number_slip_systems);
  _crss_solute[_qp].resize(_number_slip_systems);
  _ratio[_qp].resize(_number_slip_systems);
  _epsilon_p[_qp].zero();     
  _epsilon_e[_qp].zero();    
  _epsilon_total[_qp].zero();
  _slip_resistance_gamma[_qp].resize(_number_slip_systems);
  _slip_resistance_gamma_prime[_qp].resize(_number_slip_systems);
  _damage[_qp].resize(_number_slip_systems);

  for (const auto i : make_range(_number_slip_systems))
  {
    _residual_ssd[_qp][i] = 0.0;
    _crss_solute[_qp][i] = 0.0;
    _acc_slip[_qp][i]=0.0;
    _crss_or[_qp][i]=0.0;
    _crss_gp_shear[_qp][i]=0.0;
    _slip_resistance_gamma_prime[_qp][i]=0.1;
  }
  _SB_initiation_stress[_qp]=0.0;
  _SB_initiation_strain[_qp]=0.0;
  _SB_initiation[_qp]=0;
  _inside_sb_region[_qp] = 0.0;
  _DD[_qp]=0;
}

// Calculate Schmid tensor and
// store edge and screw slip directions to calculate directional derivatives
// of the plastic slip rate
void
CrystalPlasticityDislocationDendrite::calculateSchmidTensor(
    const unsigned int & number_slip_systems,
    const std::vector<RealVectorValue> & plane_normal_vector,
    const std::vector<RealVectorValue> & direction_vector,
    std::vector<RankTwoTensor> & schmid_tensor,
    const RankTwoTensor & crysrot)
{
  std::vector<RealVectorValue> local_direction_vector, local_plane_normal;
  local_direction_vector.resize(number_slip_systems);
  local_plane_normal.resize(number_slip_systems);
  // Temporary directions and normals to calculate
  // screw dislocation slip direction
  RealVectorValue temp_mo;
  RealVectorValue temp_no;
  RealVectorValue temp_screw_mo;

  for (const auto i : make_range(_number_slip_systems))
  {
    local_direction_vector[i].zero();
    local_plane_normal[i].zero();

    // Update slip direction and normal with crystal orientation
    for (const auto j : make_range(LIBMESH_DIM))
      for (const auto k : make_range(LIBMESH_DIM))
      {
        local_direction_vector[i](j) =
            local_direction_vector[i](j) + crysrot(j, k) * direction_vector[i](k);

        local_plane_normal[i](j) =
            local_plane_normal[i](j) + crysrot(j, k) * plane_normal_vector[i](k);
      }

    // Calculate Schmid tensor
    for (const auto j : make_range(LIBMESH_DIM))
      for (const auto k : make_range(LIBMESH_DIM))
      {
        schmid_tensor[i](j, k) = local_direction_vector[i](j) * local_plane_normal[i](k);
      }
  }
  
  // Calculate and store edge and screw slip directions are also assigned
  _edge_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);
  _screw_slip_direction[_qp].resize(LIBMESH_DIM * _number_slip_systems);
  
  for (const auto i : make_range(_number_slip_systems)) {
	for (const auto j : make_range(LIBMESH_DIM)) {
	  _edge_slip_direction[_qp][i * LIBMESH_DIM + j] = local_direction_vector[i](j);
	} 
  }
  
  for (const auto i : make_range(_number_slip_systems)) {
    for (const auto j : make_range(LIBMESH_DIM)) {
      temp_mo(j) = local_direction_vector[i](j);
	    temp_no(j) = local_plane_normal[i](j);
    }  
    // calculate screw slip direction for this slip system
	  // and store it in the screw slip direction vector
	  temp_screw_mo = temp_mo.cross(temp_no);
    for (const auto j : make_range(LIBMESH_DIM)) {
      _screw_slip_direction[_qp][i * LIBMESH_DIM + j] = temp_screw_mo(j);
    }
  }
}

void
CrystalPlasticityDislocationDendrite::setInitialConstitutiveVariableValues()
{
  // Initialize state variables with the value at the previous time step
  _SB_initiation_stress[_qp]= _state_variable[_qp];
  _SB_initiation_strain[_qp]= _state_variable2[_qp];
  _DD[_qp]=0;
  _SB_initiation[_qp]=0;

  _rho_ssd[_qp] = _rho_ssd_old[_qp];
  _previous_substep_rho_ssd = _rho_ssd_old[_qp];
  _rho_gnd_edge[_qp] = _rho_gnd_edge_old[_qp];
  _previous_substep_rho_gnd_edge = _rho_gnd_edge_old[_qp];
  _rho_gnd_screw[_qp] = _rho_gnd_screw_old[_qp];
  _previous_substep_rho_gnd_screw = _rho_gnd_screw_old[_qp];
  _backstress[_qp] = _backstress_old[_qp];
  _previous_substep_backstress = _backstress_old[_qp];

  _damage[_qp] = _damage_old[_qp];
  _previous_substep_damage = _damage_old[_qp];

    if (_ave_damage[_qp] > _Dcr) 
    {
      mooseError("reach the critical damage");
    }

}

void
CrystalPlasticityDislocationDendrite::setSubstepConstitutiveVariableValues()
{
  // Inialize state variable of the next substep
  // with the value at the previous substep
  initial_microstructure_crss();
  initial_segregation_crss();
  if (_with_residual_dis == 1) initial_gss();
  _DD[_qp]=0;
  _SB_initiation[_qp]=0;

  for (const auto i : make_range(_number_slip_systems)) _ratio[_qp][i]=0.0;

  _rho_ssd[_qp] = _previous_substep_rho_ssd;
  _rho_gnd_edge[_qp] = _previous_substep_rho_gnd_edge;
  _rho_gnd_screw[_qp] = _previous_substep_rho_gnd_screw;
  _backstress[_qp] = _previous_substep_backstress;
  _damage[_qp] = _previous_substep_damage;
}

// Slip resistance can be calculated from dislocation density here only
// because it is the first method in which it is used,
// while calculateConstitutiveSlipDerivative is called afterwards
bool
CrystalPlasticityDislocationDendrite::calculateSlipRate()
{
  calculateSlipResistance();
  
  // Ratio between effective stress and CRSS
  // temporary variable for each slip system
  Real stress_ratio;

  // Difference between RSS and backstress
  // temporary variable for each slip system
  Real effective_stress;

  // Slip prefactor: if function is not given
  // the constant value is used
  Real creep_ao;
  
  if (_creep_ao_function) {
    creep_ao = _creep_ao_function->value(_t, _q_point[_qp]);
  } else {
    creep_ao = _creep_ao;
  }
   // Strain rate sensitivity: if material property is not given
   // the constant value is used
  Real xm;
  if (_include_xm_matprop) {
    xm = (*_xm_matprop)[_qp];	  
  } else {
    xm = _xm; 
  }
  
  for (const auto i : make_range(_number_slip_systems))
  {
      CalSlipResistanceGamma(i);
      
      Real s_g=0.0;
      Real s_gp=0.0;
      s_g=_ao*pow((abs(_tau[_qp][i]))/_slip_resistance_gamma[_qp][i],1/_xm);

      if (_inside_sb_region[_qp]==1 && i<3) {//(111) slip system
       s_gp=_ao2*pow((abs(_tau[_qp][i]))/(_slip_resistance_gamma_prime[_qp][i]),1/_xm2);     
      }

      _slip_increment[_qp][i]=(1-_gp_fr[_qp])*s_g+_gp_fr[_qp]*s_gp;

      if (_tau[_qp][i] < 0.0) _slip_increment[_qp][i] *= -1.0; 

      if (std::abs(_slip_increment[_qp][i]) * _substep_dt > _slip_incr_tol)
      {
        if (_print_convergence_message)
          mooseWarning("Maximum allowable slip increment exceeded ",
                      std::abs(_slip_increment[_qp][i]) * _substep_dt);
        return false;
      }
  }
  return true;
}

// Slip resistance based on Taylor hardening
void
CrystalPlasticityDislocationDendrite::calculateSlipResistance()
{
  Real taylor_hardening;

  for (const auto i : make_range(_number_slip_systems))
  {
    _slip_resistance[_qp][i] = _tau_c_0;
    taylor_hardening = 0.0;
	  
    for (const auto j : make_range(_number_slip_systems))
    {
      unsigned int iplane, jplane;
      // Determine slip planes
      iplane = i / 3;
      jplane = j / 3;

      if (iplane == jplane) {  // q_{ab} = 1.0 for self hardening
	    taylor_hardening += (_rho_ssd[_qp][j] + _residual_ssd[_qp][j]
		          + std::abs(_rho_gnd_edge[_qp][j])
				  + std::abs(_rho_gnd_screw[_qp][j])); 
		  
	    } else {  // latent hardening
	    taylor_hardening += (_r * (_rho_ssd[_qp][j] + _residual_ssd[_qp][j]
		          + std::abs(_rho_gnd_edge[_qp][j])
				  + std::abs(_rho_gnd_screw[_qp][j])));
	  }
    }
	
	_slip_resistance[_qp][i] += (_alpha_0 * _shear_modulus * _burgers_vector_mag
	                          * std::sqrt(taylor_hardening));
  }

}

void
CrystalPlasticityDislocationDendrite::calculateEquivalentSlipIncrement(
    RankTwoTensor & equivalent_slip_increment)
{
  if (_include_twinning_in_Lp)
  {
    for (const auto i : make_range(_number_slip_systems))
      equivalent_slip_increment += (1.0 - (*_twin_volume_fraction_total)[_qp]) *
                                   _flow_direction[_qp][i] * _slip_increment[_qp][i] * _substep_dt;
  }
  else 
    CrystalPlasticityDislocationDendriteBase::calculateEquivalentSlipIncrement(equivalent_slip_increment);
}

// Note that this is always called after calculateSlipRate
// because calculateSlipRate is called in calculateResidual
// while this is called in calculateJacobian
// therefore it is ok to calculate calculateSlipRate
// only inside calculateSlipRate
void
CrystalPlasticityDislocationDendrite::calculateConstitutiveSlipDerivative(
    std::vector<Real> & dslip_dtau)
{
  Real stress_ratio;
  Real effective_stress;
  Real creep_ao;
  
  if (_creep_ao_function) {
    creep_ao = _creep_ao_function->value(_t, _q_point[_qp]);
  } else {  
    creep_ao = _creep_ao;
  }	
  
  Real xm;
  if (_include_xm_matprop) {
    xm = (*_xm_matprop)[_qp];	  
  } else {
    xm = _xm;
  }
	
  for (const auto i : make_range(_number_slip_systems))
  {
    CalSlipResistanceGamma(i);
    Real ds_g=0.0;
    Real ds_gp=0.0;
    if (MooseUtils::absoluteFuzzyEqual(_tau[_qp][i], 0.0))   dslip_dtau[i] = 0.0;
        ds_g=_ao*(1/_xm)/(_slip_resistance_gamma[_qp][i])*pow((abs(_tau[_qp][i]))/_slip_resistance_gamma[_qp][i],(1/_xm-1));
      if (_inside_sb_region[_qp]==1 && i<3){
        ds_gp=_ao2*(1/_xm2)/(_slip_resistance_gamma_prime[_qp][i])*pow((abs(_tau[_qp][i]))/_slip_resistance_gamma_prime[_qp][i],(1/_xm2-1));
      }
     dslip_dtau[i] = (1-_gp_fr[_qp])*ds_g + _gp_fr[_qp]*ds_gp;
  }
}

bool
CrystalPlasticityDislocationDendrite::areConstitutiveStateVariablesConverged()
{
  return isConstitutiveStateVariableConverged(_rho_ssd[_qp],
                                              _rho_ssd_before_update,
                                              _previous_substep_rho_ssd,
                                              _rho_tol);
}

void
CrystalPlasticityDislocationDendrite::updateSubstepConstitutiveVariableValues()
{
  _previous_substep_rho_ssd = _rho_ssd[_qp];
  _previous_substep_rho_gnd_edge = _rho_gnd_edge[_qp];
  _previous_substep_rho_gnd_screw = _rho_gnd_screw[_qp];
  _previous_substep_backstress = _backstress[_qp];
  _previous_substep_damage = _damage[_qp];
}

void
CrystalPlasticityDislocationDendrite::cacheStateVariablesBeforeUpdate()
{
  _rho_ssd_before_update = _rho_ssd[_qp];
  _rho_gnd_edge_before_update = _rho_gnd_edge[_qp];
  _rho_gnd_screw_before_update = _rho_gnd_screw[_qp];
  _backstress_before_update = _backstress[_qp];
  _damage_before_update=_damage[_qp];
}

void
CrystalPlasticityDislocationDendrite::calculateStateVariableEvolutionRateComponent()
{
  Real rho_sum;

  // SSD dislocation density increment
  for (const auto i : make_range(_number_slip_systems))
  {
    rho_sum = _rho_ssd[_qp][i] + std::abs(_rho_gnd_edge[_qp][i]) + std::abs(_rho_gnd_screw[_qp][i]) + _residual_ssd[_qp][i];
    // Multiplication and annihilation
	  // note that _slip_increment here is the rate
	  // and the rate equation gets multiplied by time step in updateStateVariables
    _rho_ssd_increment[i] = _k_0 * sqrt(rho_sum) - 2 * _y_c * _rho_ssd[_qp][i];
    _rho_ssd_increment[i] *= std::abs(_slip_increment[_qp][i]) / _burgers_vector_mag;
  }
    // GND dislocation density increment
  for (const auto i : make_range(_number_slip_systems)) 
  {
   if (_with_GND==1) {
    _rho_gnd_edge_increment[i] = (-1.0) * _dslip_increment_dedge[_qp](i) / _burgers_vector_mag / _scale;
    _rho_gnd_screw_increment[i] = _dslip_increment_dscrew[_qp](i) / _burgers_vector_mag / _scale;
   }
   else {
    _rho_gnd_edge_increment[i] = 0;
    _rho_gnd_screw_increment[i] = 0;
   }
  }
  damage_increment();
  // backstress increment
  ArmstrongFrederickBackstressUpdate();
}

void
CrystalPlasticityDislocationDendrite::ArmstrongFrederickBackstressUpdate()
{
  for (const auto i : make_range(_number_slip_systems)) 
  {
    _backstress_increment[i] = _h * _slip_increment[_qp][i];
    _backstress_increment[i] -= _h_D * _backstress[_qp][i] * std::abs(_slip_increment[_qp][i]);  
  }
}

bool
CrystalPlasticityDislocationDendrite::updateStateVariables()
{
  SB_evolution();
  strain_calculation();
  damage_update();

  for (const auto i : make_range(_number_slip_systems))
  { 
    _rho_ssd_increment[i] *= _substep_dt;
    if (_previous_substep_rho_ssd[i] < _zero_tol && _rho_ssd_increment[i] < 0.0)
      _rho_ssd[_qp][i] = _previous_substep_rho_ssd[i];
    else
      _rho_ssd[_qp][i] = _previous_substep_rho_ssd[i] + _rho_ssd_increment[i];

    if (_rho_ssd[_qp][i] < 0.0)
      return false;
  }
  
  for (const auto i : make_range(_number_slip_systems))
  { 
    _rho_gnd_edge_increment[i] *= _substep_dt;
    _rho_gnd_edge[_qp][i] = _previous_substep_rho_gnd_edge[i] + _rho_gnd_edge_increment[i];
  }
  
  for (const auto i : make_range(_number_slip_systems))
  { 
    _rho_gnd_screw_increment[i] *= _substep_dt;
    _rho_gnd_screw[_qp][i] = _previous_substep_rho_gnd_screw[i] + _rho_gnd_screw_increment[i];
  }

  for (const auto i : make_range(_number_slip_systems))
  { 
    _backstress_increment[i] *= _substep_dt;
    _backstress[_qp][i] = _previous_substep_backstress[i] + _backstress_increment[i];
  }
  
  return true;
}

double CrystalPlasticityDislocationDendrite::local_dimensionless_r(){
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
  if (_q_point[_qp](1)==0 && _q_point[_qp](0)==0) return 0;
  return rm/r0;
}

void CrystalPlasticityDislocationDendrite::initial_segregation_crss() 
{
// Al and Ti are not involved, since they mainly contribute to the formation of gamma prime phases, and are only marginally distributed within gamma matrix (Reed, 2008)
  double k_al = 225;
  double k_co = 39.4;
  double k_cr = 337;
  double k_mo = 1015;
  double k_ti = 775;
         
  for (const auto i : make_range(5)){
    _conc_ele[_qp][i] = _read_conc_ele->getData(_current_elem, i);
  }
         double _c_al = _conc_ele[_qp][0];
         double _c_co = _conc_ele[_qp][1];
         double _c_cr = _conc_ele[_qp][2];
         double _c_mo = _conc_ele[_qp][3];
         double _c_ti = _conc_ele[_qp][4];
         double sum=k_co*k_co*_c_co+k_cr*k_cr*_c_cr+k_mo*k_mo*_c_mo;
  for (const auto i : make_range(_number_slip_systems)) {
  _crss_solute[_qp][i]=pow(sum,0.5)*0.272;
  }
}

void CrystalPlasticityDislocationDendrite::initial_microstructure_crss()
{
  for (const auto i : make_range(5)){
    _micro_morph[_qp][i] = _read_micro_morph->getData(_current_elem, i);
  }

  _gp_H[_qp] = _micro_morph[_qp][0];
  _gp_L[_qp] = _micro_morph[_qp][1];
  _g_wH[_qp] = _micro_morph[_qp][2];
  _g_wL[_qp] = _micro_morph[_qp][3];
  _gp_fr[_qp] = _micro_morph[_qp][4];

  double rr0 = std::pow(( _gp_L[_qp]*_gp_L[_qp]*_gp_H[_qp] / 3.1415 * 3 / 4 ), (0.333));
    for (int i = 0; i < _number_slip_systems; i++) {
       _crss_gp_shear[_qp][i] = _gamma_APB / (2 * _burgers_vector_mag / 1E+6) * (std::pow((6 * _gamma_APB * _gp_fr[_qp] * rr0 / 1E+9 / 3.1415 / (0.5 * _G_shear * 1E+6 * pow(_burgers_vector_mag / 1E+6, 2))), 0.5) - _gp_fr[_qp]) / 1E+6;         
       _crss_or[_qp][i] = _G_shear * _burgers_vector_mag * 1000 / _g_wL[_qp];
      }
}

void CrystalPlasticityDislocationDendrite::initial_gss(){
      RankTwoTensor _residual_strain;
      _residual_strain = _residual_eigenstrain[_qp];
      _residual_strain.rotate(_crysrot[_qp].transpose()); //from global to crystal coordinate 
      _temp_gss[0] = abs( _residual_strain(2,2) )/3.2;
      _temp_gss[1] = abs( _residual_strain(2,2) )/3.2;
      _temp_gss[2] = 0;
      _temp_gss[3] = abs( _residual_strain(2,2) )/3.2;
      _temp_gss[4] = abs( _residual_strain(2,2) )/3.2;
      _temp_gss[5] = 0;
      _temp_gss[6] = abs( _residual_strain(2,2) )/3.2;
      _temp_gss[7] = abs( _residual_strain(2,2) )/3.2;
      _temp_gss[8] = 0;
      _temp_gss[9] = abs( _residual_strain(2,2) )/3.2;
      _temp_gss[10] = abs( _residual_strain(2,2) )/3.2;
      _temp_gss[11] = 0;
      for (const auto i : make_range(_number_slip_systems))
      {
        if (_t>1){
             // pre_factor 150 to capture the initial dislocation hardening accomodate the residual deformation, calibrated using as-printed and heat-treated states
            _residual_ssd[_qp][i] = pow((abs(_temp_gss[i])/_burgers_vector_mag*_k_0),2) / 2 * 150;
        }
      }
}

void CrystalPlasticityDislocationDendrite::SB_evolution(){

  double _initial_SB_width0 = _initial_SB_width / 6 * _z_length;
  RankTwoTensor temp;
  bool core = false;

  if (_q_point[_qp].norm() < 0.1 && abs(_q_point[_qp](2)) < 1)
  {
    core = true;
  }

  double D0 = A_sb * _q_point[_qp](0) + B_sb * _q_point[_qp](1) + C_sb * _q_point[_qp](2);
  if (D0 >= _D1_old && D0 <= _D2_old  && (_D1_old != 0 || _D2_old != 0) )//
  {
    _inside_sb_region[_qp] = 1;
  }

  if (_sb_initiation_old>0){
      _SB_initiation[_qp] = 1;
  }

    for (int i = 0; i < 3; i++)// (111) slip plane
    {
      double _CRSS_sb=_crss_gp_shear[_qp][i];//
      if (abs(_tau[_qp][i]) > _CRSS_sb && ( core == true ) && ( _t > 5 ))
      {
          _SB_initiation[_qp] = 1;
          activited_slip_sys = i;
          if (i < 3)
          {
            temp(0, 0) = 1;
            temp(0, 1) = 1;
            temp(0, 2) = 1;
          }
          _initial_D1= -1 *_initial_SB_width0;
          _initial_D2=     _initial_SB_width0;
      }
    }
      if (_sb_initiation_old==1){// to capture slip band propagation
         if ( local_dimensionless_r()<0.3 && _SB_initiation_stress[_qp]>0 ){//propagation
            double threshold = _initial_SB_width0 + (_stress_zz_old - _SB_initiation_stress[_qp])*_sb_evo_rate*(_z_length/2);//evolution rate of the threshold
            double temp = threshold;
              if (abs(D0) < temp ){
                _DD[_qp] = D0;
              }
         }
         if (D0>=_initial_D1 && D0<=0) _DD[_qp]=_initial_D1;//initialization
         if (D0<=_initial_D2 && D0>=0) _DD[_qp]=_initial_D2;//initialization
      }
}

void CrystalPlasticityDislocationDendrite::CalSlipResistanceGamma(int i)
{
     if (_inside_sb_region[_qp]==1 && i<3 ){//(111) slip plane && (abs(_tau[_qp][i])>=_crss_gp_shear[_qp][i])
        _ratio[_qp][i]=pow(abs((_strain_zz_old - _SB_initiation_strain[_qp])/( _end_strain - _SB_initiation_strain[_qp])), 1);
        if (_ratio[_qp][i]>1) _ratio[_qp][i]=1;
        if (_ratio[_qp][i]<0) _ratio[_qp][i]=0;
      double temp0;
      double temp1 = _initial_shear + _ratio[_qp][i]*(_crss_gp_shear[_qp][i] - _initial_shear);
      double r0=( _min_crss_shear - _initial_shear ) / ( _max_crss_shear - _initial_shear );
      double r1=1;
      if (temp1 <= _crss_gp_shear[_qp][i]) {
           temp0 = temp1;
      }
      else {
           temp0 = _crss_gp_shear[_qp][i]; 
      }
      _slip_resistance_gamma[_qp][i] = (1-_damage[_qp][i]) * ( _slip_resistance[_qp][i] + _crss_solute[_qp][i] + (1- temp1/_crss_gp_shear[_qp][i]) * _crss_or[_qp][i]);
      _slip_resistance_gamma_prime[_qp][i] = (1-_damage[_qp][i]) * ( _tau_c_0 + temp1 ); 
   }
   else {
    _slip_resistance_gamma[_qp][i] = (1-_damage[_qp][i]) * ( _slip_resistance[_qp][i] + _crss_or[_qp][i] + _crss_solute[_qp][i]);
    _slip_resistance_gamma_prime[_qp][i] = (1-_damage[_qp][i]) * ( _initial_shear );
   }
}

void CrystalPlasticityDislocationDendrite::strain_calculation()
{
  _epsilon_p[_qp] = _plastic_deformation_gradient[_qp].transpose() * _plastic_deformation_gradient[_qp];
  _epsilon_p[_qp] = _epsilon_p[_qp] - RankTwoTensor::Identity();
  _epsilon_p[_qp] *= 0.5;

  if (abs(_epsilon_p[_qp](2,2))<1E-10) _epsilon_p[_qp](2,2)=1E-10;

  _epsilon_total[_qp] = _deformation_gradient[_qp].transpose() * _deformation_gradient[_qp];
  _epsilon_total[_qp] = _epsilon_total[_qp] - RankTwoTensor::Identity();
  _epsilon_total[_qp] *= 0.5;

  if (abs(_epsilon_total[_qp](2,2))<1E-10) _epsilon_total[_qp](2,2)=1E-10;

  _epsilon_e[_qp] = _epsilon_total[_qp] - _epsilon_p[_qp];

  RankTwoTensor _rotation;
  _rotation(0,0) = 0.7071;_rotation(0,1) = 0.7071;
  _rotation(1,0) = -0.7071;_rotation(1,1) = 0.7071;
  _rotation(2,2) = 1;

  RankTwoTensor _temp;
  _temp=_epsilon_total[_qp];
  _temp.rotate(_rotation);
  _110_yy[_qp] = _temp (1,1);
  _110_xy[_qp] = _temp (1,2);

  if (_t>1 &&  _ini_e_total[_qp](2,2)==0){
    _ini_e_total[_qp] = _epsilon_total[_qp];
    _ini_e_p[_qp] = _epsilon_p[_qp];
  }
}

void CrystalPlasticityDislocationDendrite::damage_update()
{
  Real _sum0 = 0;
  for (const auto i : make_range(_number_slip_systems))
  {
     _damage[_qp][i]=_previous_substep_damage[i]+_damage_increment[i];
     _sum0 += _damage[_qp][i];
  }
  _ave_damage[_qp] = _sum0 ;
}

void CrystalPlasticityDislocationDendrite::damage_increment()
{
  Real ssd = pow(pow( _rho_ssd[_qp][0] + _rho_ssd[_qp][1] + _rho_ssd[_qp][2] + _rho_ssd[_qp][3] + _rho_ssd[_qp][4] + _rho_ssd[_qp][5] + _rho_ssd[_qp][6] + _rho_ssd[_qp][7] + _rho_ssd[_qp][8] + _rho_ssd[_qp][9] + _rho_ssd[_qp][10] + _rho_ssd[_qp][11],2),0.5);
   for (const auto i : make_range(_number_slip_systems))
   {
      _damage_increment[i]=std::abs(_slip_increment[_qp][i]*_tau[_qp][i]*_substep_dt)/_Wcr; 
   }
}