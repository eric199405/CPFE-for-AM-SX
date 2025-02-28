#pragma once
#include "CrystalPlasticityDislocationUpdateBase.h"
#include "ElementPropertyReadFile.h"

class CrystalPlasticityDislocationUpdate;
/**
 * CrystalPlasticityDislocationUpdate uses the multiplicative decomposition of the
 * deformation gradient and solves the PK2 stress residual equation at the
 * intermediate configuration to evolve the material state. The internal
 * variables are updated using an interative predictor-corrector algorithm.
 * Backward Euler integration rule is used for the rate equations.
 * Dislocation based model for crystal plasticity.
 * Slip, creep and backstress are included 
 */
class CrystalPlasticityDislocationUpdate : public CrystalPlasticityDislocationUpdateBase
{
public:
  static InputParameters validParams();

  CrystalPlasticityDislocationUpdate(const InputParameters & parameters);

protected:
  /**
   * initializes the stateful properties such as
   * stress, plastic deformation gradient, slip system resistances, etc.
   */
  virtual void initQpStatefulProperties() override;
  /**
   * A helper method to rotate the a direction and plane normal system set into
   * the local crystal lattice orientation as defined by the crystal rotation
   * tensor from the Elasticity tensor class.
   * Edge and screw slip directions are also assigned
   */
  virtual void calculateSchmidTensor(const unsigned int & number_dislocation_systems,
                             const std::vector<RealVectorValue> & plane_normal_vector,
                             const std::vector<RealVectorValue> & direction_vector,
                             std::vector<RankTwoTensor> & schmid_tensor,
                             const RankTwoTensor & crysrot);
  /**
   * Sets the value of the current and previous substep iteration slip system
   * resistance to the old value at the start of the PK2 stress convergence
   * while loop.
   */
  virtual void setInitialConstitutiveVariableValues() override;
  /**
   * Sets the current slip system resistance value to the previous substep value.
   * In cases where only one substep is taken (or when the first) substep is taken,
   * this method just sets the current value to the old slip system resistance
   * value again.
   */
  virtual void setSubstepConstitutiveVariableValues() override;
  /**
   * Stores the current value of the slip system resistance into a separate
   * material property in case substepping is needed.
   */
  virtual void updateSubstepConstitutiveVariableValues() override;

  virtual bool calculateSlipRate() override;
  
  virtual void calculateSlipResistance();

  virtual void
  calculateEquivalentSlipIncrement(RankTwoTensor & /*equivalent_slip_increment*/) override;

  virtual void calculateConstitutiveSlipDerivative(std::vector<Real> & dslip_dtau) override;

  // Cache the slip system value before the update for the diff in the convergence check
  virtual void cacheStateVariablesBeforeUpdate() override;
  /**
   * Following the Constitutive model for slip system resistance as given in
   * Dislocation based model:
   * E. Demir, I Gutierrez-Urrutia
   * Investigation of strain hardening near grain
   * boundaries of an aluminium oligocrystal:
   * Experiments and crystal based finite element method
   * International Journal of Plasticity 136 (2021) 102898
   */
  virtual void calculateStateVariableEvolutionRateComponent() override;
  // Armstrong-Frederick update of the backstress
  virtual void ArmstrongFrederickBackstressUpdate();
  /*
   * Finalizes the values of the state variables and slip system resistance
   * for the current timestep after convergence has been reached.
   */
  virtual bool updateStateVariables() override;
  /*
   * Determines if the state variables, e.g. defect densities, have converged
   * by comparing the change in the values over the iteration period.
   */
  virtual bool areConstitutiveStateVariablesConverged() override;

  // Variables used in
  // Eralp Demir, Ivan Gutierrez-Urrutia
  // Investigation of strain hardening near grain boundaries of an aluminum oligocrystal: 
  // Experiments and crystal based finite element method
  // International Journal of Plasticity
  // Volume 136, January 2021, 102898
  // Slip rate constants
  const Real _ao;
  const Real _xm;
  const Real _ao2;
  const Real _xm2;
  
  // Optional xm material property for exponent for slip rate
  const bool _include_xm_matprop;
  const MaterialProperty<Real> * _xm_matprop;
  
  // Creep rate constants
  const Real _creep_ao;
  const Real _creep_xm;
  
  // Optional function for creep prefactor. If provided, the creep prefactor can be set as a function of time
  // This is useful for an initial plastic deformation followed by creep load
  const Function * const _creep_ao_function;
  
  // Cap the absolute value of the slip increment in one time step to _slip_incr_tol
  const bool _cap_slip_increment;

  // Magnitude of the Burgers vector
  const Real _burgers_vector_mag;
  
  // Shear modulus in Taylor hardening law G
  const Real _shear_modulus;
  
  // Prefactor of Taylor hardening law, alpha
  const Real _alpha_0;
  
  // Latent hardening coefficient
  const Real _r;
  
  // Peierls stress
  const Real _tau_c_0;
  
  // Coefficient K in SSD evolution, representing accumulation rate
  const Real _k_0;
  
  // Critical annihilation diameter
  const Real _y_c;
  
  // Backstress parameters
  const Real _h;
  const Real _h_D;
  
  // Initial values of the dislocation density
  const Real _init_rho_ssd;
  const Real _init_rho_gnd_edge;
  const Real _init_rho_gnd_screw;
  
  // Tolerance on dislocation density update
  const Real _rho_tol;
// update by zixu
  std::vector<Real> _temp_gss;
  std::vector<Real> _temp_strain;
  const Real _scale;// 
  const Real _pre_factor_residual;// 
  const Real _gamma_APB;//
  const Real _G_shear;//
  const Real _xita;
  const Real _k_solute;// 
  const Real _k_HL;// 
  const Real _k_w;//
  const int _inhomo_or;
  const int _inhomo_shear;
  const int _inhomo_solute;
  const int _with_GND;
  const int _with_residual_dis;
  const Real _z_length;// -/MPa
  const Real _sb_evo_rate;
  const Real _initial_SB_width;
  const Real _Wcr;//MPa
  const Real _Dcr;//MPa
  const PostprocessorValue & _D1_old;
  const PostprocessorValue & _D2_old;
  const PostprocessorValue & _sb_initiation_old;
  const PostprocessorValue & _stress_zz_old;
  const PostprocessorValue & _strain_zz_old;
  const PostprocessorValue & _max_crss_shear;
  const PostprocessorValue & _min_crss_shear;
  Real _initial_D1=0;
  Real _initial_D2=0;
  Real  A_sb = -1;
  Real  B_sb = 1;
  Real  C_sb = 1;
  const Real _initial_shear;
  const Real _end_strain;
//
//
//
  // Dislocation densities
  MaterialProperty<std::vector<Real>> & _rho_ssd;
  const MaterialProperty<std::vector<Real>> & _rho_ssd_old;
  MaterialProperty<std::vector<Real>> & _rho_gnd_edge;
  const MaterialProperty<std::vector<Real>> & _rho_gnd_edge_old;
  MaterialProperty<std::vector<Real>> & _rho_gnd_screw;
  const MaterialProperty<std::vector<Real>> & _rho_gnd_screw_old;
  
  // Backstress variables
  MaterialProperty<std::vector<Real>> & _backstress;
  const MaterialProperty<std::vector<Real>> & _backstress_old;  
  
  /// Increment of dislocation densities and backstress
  std::vector<Real> _rho_ssd_increment;
  std::vector<Real> _rho_gnd_edge_increment;
  std::vector<Real> _rho_gnd_screw_increment;
  std::vector<Real> _backstress_increment;
  /**
   * Stores the values of the dislocation densities and backstress 
   * from the previous substep
   * In classes which use dislocation densities, analogous dislocation density
   * substep vectors will be required.
   */
  std::vector<Real> _previous_substep_rho_ssd;
  std::vector<Real> _previous_substep_rho_gnd_edge;
  std::vector<Real> _previous_substep_rho_gnd_screw;
  std::vector<Real> _previous_substep_backstress;
  std::vector<Real> _damage_increment;
  std::vector<Real> _previous_substep_damage;
  /**
   * Caches the value of the current dislocation densities immediately prior
   * to the update, and they are used to calculate the
   * the dislocation densities for the current substep (or step if
   * only one substep is taken) for the convergence check tolerance comparison.
   */
  std::vector<Real> _rho_ssd_before_update;
  std::vector<Real> _rho_gnd_edge_before_update; 
  std::vector<Real> _rho_gnd_screw_before_update;
  std::vector<Real> _backstress_before_update;
  std::vector<Real> _damage_before_update;
  /**
   * Flag to include the total twin volume fraction in the plastic velocity
   * gradient calculation, per Kalidindi IJP (2001).
   */
  const bool _include_twinning_in_Lp;
  /**
   * User-defined material property name for the total volume fraction of twins
   * in a twinning propagation constitutive model, when this class is used in
   * conjunction with the twinning propagation model.
   * Note that this value is the OLD material property and thus lags the current
   * value by a single timestep.
   */
  const MaterialProperty<Real> * const _twin_volume_fraction_total;
  /**
   * UserObject to read the initial GND density from file
   */
  const ElementPropertyReadFile * const _read_initial_gnd_density;
  // Directional derivative of the slip rate along the edge dislocation motion direction
  // and along the screw dislocation motion direction
  const ArrayVariableValue & _dslip_increment_dedge;
  const ArrayVariableValue & _dslip_increment_dscrew;
  // Temperature dependent properties
  const VariableValue & _temperature;
  const Real _reference_temperature;
  const Real _dCRSS_dT_A;
  const Real _dCRSS_dT_B;
  const Real _dCRSS_dT_C;
  // Rotated slip direction to calculate the directional derivative
  // of the plastic strain rate
  // it indicates the edge dislocation velocity direction for all slip systems
  MaterialProperty<std::vector<Real>> & _edge_slip_direction;
  // edge dislocation line direction
  // corresponding to direction of motion of screw dislocations
  MaterialProperty<std::vector<Real>> & _screw_slip_direction;
  // To consider the gradient microstructure
  void initial_segregation_crss(); // define the initial crss0 induced by element segregation 
  void initial_microstructure_crss(); // define the initial geometry parameters here
  void initial_gss();
  double local_dimensionless_r();
  void SB_evolution();
  void CalSlipResistanceGamma(int i);
  void strain_calculation();
  void damage_update();
  void damage_increment();

  MaterialProperty<std::vector<Real>> & _acc_slip;
    const MaterialProperty<std::vector<Real>> & _acc_slip_old;
  MaterialProperty<std::vector<Real>> & _crss_or;
  MaterialProperty<std::vector<Real>> & _crss_gp_shear;
  MaterialProperty<std::vector<Real>> & _residual_ssd;
  MaterialProperty<std::vector<Real>> & _crss_solute;
  MaterialProperty<Real> & _c_al;
  MaterialProperty<Real> & _gp_H;//GEOMETRY PARAMETERS
  MaterialProperty<Real> & _gp_L;
  MaterialProperty<Real> & _g_wH;
  MaterialProperty<Real> & _g_wL;
  MaterialProperty<Real> & _gp_fr;
    const MaterialProperty<RankTwoTensor> & _crysrot;
    const MaterialProperty<RankTwoTensor> & _thermal_eigenstrain;

  MaterialProperty<Real> & _110_yy;
  MaterialProperty<Real> & _110_xy;
  MaterialProperty<Real> & _DD;
  MaterialProperty<Real> & _SB_initiation;

  MaterialProperty<Real> & _SB_initiation_stress;
  MaterialProperty<Real> & _SB_initiation_strain;
  MaterialProperty<Real> & _ave_damage;
  MaterialProperty<RankTwoTensor> & _epsilon_p;
  MaterialProperty<RankTwoTensor> & _epsilon_e;
  MaterialProperty<RankTwoTensor> & _epsilon_total;
  MaterialProperty<Real> & _inside_sb_region;
   const MaterialProperty<Real> & _inside_sb_region_old;
  MaterialProperty<std::vector<Real>> & _slip_resistance_gamma;
  MaterialProperty<std::vector<Real>> & _slip_resistance_gamma_prime;
  MaterialProperty<std::vector<Real>> & _ratio;

  MaterialProperty<RankTwoTensor> & _ini_e_p;
  MaterialProperty<RankTwoTensor> & _ini_e_total;

  const MaterialProperty<RankTwoTensor> & _plastic_deformation_gradient;
  const MaterialProperty<RankTwoTensor> & _thermal_deformation_gradient;
  const MaterialProperty<RankTwoTensor> & _deformation_gradient;

  const VariableValue & _state_variable;
  const VariableValue & _state_variable2;

  MaterialProperty<std::vector<Real>> & _damage;
   const MaterialProperty<std::vector<Real>> & _damage_old;
   const MaterialProperty<std::vector<Real>> & _tau_old;

};

