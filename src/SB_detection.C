// Zixu Guo
// National University of Singapore
// Yilun Xu
// Imperial College London, IHPC A*STAR
// 01 MAY 2023

#include "SB_detection.h"

registerMooseObject("MooseApp", SB_detection);

InputParameters
SB_detection::validParams() 
{
  InputParameters params = AuxKernel::validParams();
  params.addClassDescription(
      "Calculate directional derivative along edge and screw dislocation propagation direction."
	  "This AuxKernel applies to a vector auxiliary variable");
  params.addRequiredParam<PostprocessorName>("sb_initiation_old","initiation status of slip band in the previous time step");
  params.addRequiredParam<PostprocessorName>("stress_zz_old","stress_zz in the previous time step, obtained from Postprocessor");
  params.addRequiredParam<PostprocessorName>("strain_zz_old","strain_zz in the previous time step, obtained from Postprocessor");
  params.addParam<int>("type0", 0.0, "-");
  return params;
}

SB_detection::SB_detection(const InputParameters & parameters)
  : AuxKernel(parameters),
    _sb_initiation_old(getPostprocessorValue("sb_initiation_old")), // initiation status of slip band in the previous time step
    _SB_initiation_stress(getMaterialProperty<Real>("SB_initiation_stress")), // get the slip band initiation stress 
    _SB_initiation_strain(getMaterialProperty<Real>("SB_initiation_strain")), // get the slip band initiation strain
    _stress_zz_old(getPostprocessorValue("stress_zz_old")), // stress_zz in the previous time step, obtained from Postprocessor
    _strain_zz_old(getPostprocessorValue("strain_zz_old")), // strain_zz in the previous time step, obtained from Postprocessor
    _type0(getParam<int>("type0")) // 0 stress; 1 strain
{
}

Real SB_detection::computeValue() // return the value
{
  if (_type0==0){
    if (_sb_initiation_old>0 && _SB_initiation_stress[_qp]==0) return _stress_zz_old;
    else return _SB_initiation_stress[_qp];
  }
  if (_type0==1){
    if (_sb_initiation_old>0 && _SB_initiation_strain[_qp]==0) return _strain_zz_old;
    else return _SB_initiation_strain[_qp];
  }
  return 0;
}


