#pragma once

#include "AuxKernel.h"

#include "CrystalPlasticityDislocationUpdate.h"

#include "ComputeMultipleCrystalPlasticityStress.h"


class SB_detection : public AuxKernel
{
public:
  static InputParameters validParams();

  SB_detection(const InputParameters & parameters);
  
protected:
  virtual Real computeValue() override;

  const PostprocessorValue & _sb_initiation_old;

  const MaterialProperty<Real> & _SB_initiation_stress;

  const MaterialProperty<Real> & _SB_initiation_strain;
  
  const PostprocessorValue & _stress_zz_old;

  const PostprocessorValue & _strain_zz_old;

  const int _type0;

private:


};

