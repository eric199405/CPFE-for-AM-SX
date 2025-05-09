CPFE-for-AM-SX
=====

Crystal plasticity finite element (CPFE) code for AM SX, to simulate the deformation at dendrite scale, under the open-source MOOSE software, using C++ language.  

The CPFE simulation is conducted under open-source MOOSE platform for the FEM analysis, with source code for the manuscript attached. We also provide a representative demo case in ‘CPFE_980C.i’ as an input file, corresponding to the simulation at 980 °C for the dendrite deformation considering the effects of trigonometric gradient microstructures (TGMs), based on the dislocation-based crystal plasticity model.

I. To run the CPFE simulation in MOOSE software, please do:  
Step 1: Download and install MOOSE in Linux system (https://mooseframework.inl.gov/getting_started/installation/), under LGPL v2.1 license  
Step 2: Install the APP named ‘am_sx_dendrite’ targeting the attached source code and compile by ‘make -j8’, detailed  instructions and commands please refer to https://mooseframework.inl.gov/getting_started/new_users.html  
Step 4: Run the case of ‘CPFE_980C.i’, by entering the command: ./ am_sx_dendrite-opt -i CPFE_980C.i  
Step 4: To check the time-dependent global variables, such as global stress-strain, please open the ‘CPFE_980C_case_980C_1.csv’ file. To view the CPFE simulation results using ParaView 5.12 (https://www.paraview.org/), please load the ‘CPFE_980C_case_980C_2.e’ file in Paraview  

II. The introductions of the attached files are given as follows:  
The folders of ‘src’ and ‘include’: source C++ file and header file, respectively.  
The ‘CPFE_980C.i’ is the input file of simulation under MOOSE.  
The ‘input_slip_sys.txt’ is the slip systems of FCC alloys.  
The ‘conc_ele.txt’ and ‘micro_morph.txt’ are the inputs of gradient element concentrations and gamma/gamma prime morphology.  
The ‘Makefile’ contains the enabled modules during the compiling.  

III. Detailed introductions and documentation for the variables are provided in both input file and source code. The functions of the attached C++ source codes are listed as follows:  
CrystalPlasticityDislocationDendrite: dislocation-based plastic constitutive relations for AM SX with TGMs, which is the core of crystal plasticity model for TGMs at dendrites.  
CrystalPlasticityDislocationDendriteBase: base class of CrystalPlasticityDislocationDendrite.  
ComputeDislocationCrystalPlasticityStress: update the stress tensor for dislocation-based model.  
ComputeCrystalPlasticityResidualEigenstrain: introduce the distributed residual stresses into the CPFE model with TGMs.  
ArrayDirectionalDerivative: solve the derivative of resolved shear strain for the calculation of edge and screw GND densities.  
SB_detection: to simulate the slip band initiation and propagation.  
CoupledVarDirichletBC: to apply the pre-condition of residual stresses and initial dislocation densities.  
PiecewiseFunctions: a piecewise representation of arbitrary functions for applying the pre-condition of residual stresses and initial dislocations.  

Contacts: guozixu@nus.edu.sg (Zixu Guo); xu_yilun@ihpc.a-star.edu.sg (Yilun Xu); mpeyanw@nus.edu.sg (Wentao Yan)

Related publications:  
[1] Permann, C. J. et al. MOOSE: Enabling massively parallel multiphysics simulation. SoftwareX 11, 100430 (2020). https://doi.org:https://doi.org/10.1016/j.softx.2020.100430  
[2] Guo, Z. et al. A dislocation-based damage-coupled constitutive model for single crystal superalloy: Unveiling the effect of secondary orientation on creep life of circular hole. International Journal of Plasticity 173, 103874 (2024). https://doi.org:10.1016/j.ijplas.2024.103874  
[3] Guo, Z. et al. Slip system-resolved GNDs and SEDs: A multi-scale framework for predicting crack nucleation in single-crystal metals. Acta Materialia 288, 120853 (2025). https://doi.org:https://doi.org/10.1016/j.actamat.2025.120853  
[4] Hu, D. et al. Understanding the strain localization in additively manufactured materials: Micro-scale tensile tests and crystal plasticity modeling. International Journal of Plasticity 177, 103981 (2024). https://doi.org:https://doi.org/10.1016/j.ijplas.2024.103981  
[5] Guo, Z. et al. Slip Band Evolution Behavior near Circular Hole on Single Crystal Superalloy: Experiment and Simulation. International Journal of Plasticity 165, 103600 (2023). https://doi.org:10.1016/j.ijplas.2023.103600  
