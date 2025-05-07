// Zixu Guo
// National University of Singapore
// Yilun Xu
// Imperial College London, IHPC A*STAR
// 01 MAY 2023

#pragma once
#include "AuxKernel.h"
#include "CrystalPlasticityDislocationDendrite.h"
#include "ComputeMultipleCrystalPlasticityStress.h"


class SB_detection : public AuxKernel 
{
public:
  static InputParameters validParams();

  SB_detection(const InputParameters & parameters);
  
protected:

  virtual Real computeValue() override;

  const PostprocessorValue & _sb_initiation_old; // initiation status of slip band in the previous time step

  const MaterialProperty<Real> & _SB_initiation_stress; // get the slip band initiation stress 

  const MaterialProperty<Real> & _SB_initiation_strain; // get the slip band initiation strain
  
  const PostprocessorValue & _stress_zz_old; // stress_zz in the previous time step, obtained from Postprocessor

  const PostprocessorValue & _strain_zz_old; // strain_zz in the previous time step, obtained from Postprocessor

  const int _type0; // 0 stress; 1 strain

private:


};

