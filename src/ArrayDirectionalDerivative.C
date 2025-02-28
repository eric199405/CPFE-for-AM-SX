

#include "ArrayDirectionalDerivative.h"

registerMooseObject("MooseApp", ArrayDirectionalDerivative);

InputParameters
ArrayDirectionalDerivative::validParams()
{
  InputParameters params = ArrayAuxKernel::validParams();
  params.addClassDescription(
      "Calculate directional derivative along edge and screw dislocation propagation direction."
	  "This AuxKernel applies to a vector auxiliary variable");
  params.addRequiredCoupledVar("gradient_variable",
                               "The vector variable from which to compute the directional derivative");
  MooseEnum dislo_character("edge screw", "edge");
  params.addRequiredParam<MooseEnum>("dislo_character",
                                     dislo_character,
                                     "Character of dislocations: edge or screw.");
  return params;
}

ArrayDirectionalDerivative::ArrayDirectionalDerivative(const InputParameters & parameters)
  : ArrayAuxKernel(parameters),
    _grad_variable(coupledArrayGradient("gradient_variable")),
    _edge_slip_direction(getMaterialProperty<std::vector<Real>>("edge_slip_direction")), 
	_screw_slip_direction(getMaterialProperty<std::vector<Real>>("screw_slip_direction")), 
	_dislo_character(getParam<MooseEnum>("dislo_character").getEnum<DisloCharacter>())
{
}

RealEigenVector
ArrayDirectionalDerivative::computeValue()
{
  RealEigenVector val(_var.count());

  _velocity.resize(3, 0.0);
  
  for (unsigned int i = 0; i < _var.count(); ++i){
	  
  switch (_dislo_character)
  {
    case DisloCharacter::edge:
	  for (unsigned int j = 0; j < LIBMESH_DIM; ++j)
	  {
	    _velocity[j] = _edge_slip_direction[_qp][i * LIBMESH_DIM + j]; 
	  }
	  break;
	case DisloCharacter::screw:
	  for (unsigned int j = 0; j < LIBMESH_DIM; ++j)
	  {
	    _velocity[j] = - _screw_slip_direction[_qp][i * LIBMESH_DIM + j]; // screw direction	  
	  }	
	  break;
  }  

  val(i) = 0.0;
    
  for (unsigned int j = 0; j < LIBMESH_DIM; ++j) {
    val(i) += _grad_variable[_qp](j*_var.count()+i) * _velocity[j];
  }
	
  } 
  
  return val;
}
