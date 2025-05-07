# A demo case of dendrite deformation for AM SX at 980 °„C
# Zixu Guo
# National University of Singapore
# Yilun Xu
# Imperial College London, IHPC A*STAR
# 01 MAY 2023

z_length = 6
[GlobalParams]
  displacements = 'ux uy uz'
[]

[Mesh] # initialize the mesh, unit of demensions: half dendrite spacing
  [cube] # the whole RVE
    type = GeneratedMeshGenerator
    dim = 3
    nx = 30
    ny = 30
    nz = 60
    xmax = 1 
    ymax = 1
    zmax = 6
    xmin = -1
    ymin = -1
    zmin = -6
  []
  [sub1] # define the gauge length within RVE
    input = cube
    type = SubdomainBoundingBoxGenerator
    block_id = 2
    bottom_left = '-1 -1 -3'
    top_right = '1 1 3'
  []
  [side1] # define the boundaries within gauge length
    input = sub1
    type = SideSetsAroundSubdomainGenerator
    normal = '0 0 1'
    block = 2
    new_boundary = 'front1'
  []
  [right_domain] # define the boundaries within gauge length
    input = side1
    type = SideSetsAroundSubdomainGenerator
    normal = '0 0 -1'
    block = 2
    new_boundary = 'back1'
  []
[]

[AuxVariables] # variables to extract distributions in post-processing, detailed definition and documentation of variables refer to the source code
  [plane_yy_dic]
    order = FIRST
    family = MONOMIAL
  []
  [plane_xy_dic]
    order = FIRST
    family = MONOMIAL
  []
  [epsilon_zz_dic]
    order = FIRST
    family = MONOMIAL
  []
  [copy_epsilon_zz]
    order = FIRST
    family = MONOMIAL
  []
  [copy_plane_yy]
    order = FIRST
    family = MONOMIAL
  []
  [copy_plane_xy]
    order = FIRST
    family = MONOMIAL
  []
  [copy_back]
  []
  [copy_front]
  []
  [residual_eigenstrain]
    order = CONSTANT
    family = MONOMIAL
  []
  [temperature]
    order = FIRST
    family = LAGRANGE
  []
  [./plane_yy]
    order = FIRST
    family = MONOMIAL
  [../]
  [./plane_xy]
    order = FIRST
    family = MONOMIAL
  [../]
  [./crss_or]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ave_damage]
    order = FIRST
    family = MONOMIAL
  [../]
  [./crss_gp_shear]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./crss_solute]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./SB_initiation_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ini_stress]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ini_strain]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./inside_sb_region]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./epsilon_zz]
    order = FIRST
    family = MONOMIAL
  [../]
  [./epsilon_p_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./epsilon_e_zz]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./stress_zz]
   order = CONSTANT
   family = MONOMIAL
  [../]
  [./slip_resistance_gamma_prime]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_gamma]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./applied_shear_stress]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./ratio]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./DD]
    order = CONSTANT 
    family = MONOMIAL
  [../]
  [./SB_initiation]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_increment_1]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_2]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_3]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_4]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_5]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_6]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_7]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_8]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_9]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_10]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_11]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_12]
   order = FIRST
   family = MONOMIAL
  [../]
  [./slip_increment_vector] # for the calculation of GND
    order = FIRST
    family = MONOMIAL
    components = 12
  [../]
  [./dslip_increment_dedge] # for the calculation of edge GND, based on the spatial derivative of resolved shear strain
    order = CONSTANT
    family = MONOMIAL
    components = 12
  [../]
  [./dslip_increment_dscrew] # for the calculation of screw GND, based on the spatial derivative of resolved shear strain
    order = CONSTANT
    family = MONOMIAL
    components = 12
  [../]
  [./rho_ssd_1] # SSD density
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_2]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_3]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_4]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_5]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_6]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_7]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_8]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_9]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_10]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_ssd_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_1] # edge GND density across different slip systems
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_2]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_3]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_4]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_5]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_6]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_7]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_8]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_9]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_10]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_edge_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_1] # screw GND density across different slip systems
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_2]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_3]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_4]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_5]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_6]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_7]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_8]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_9]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_10]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./rho_gnd_screw_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_1] # slip resistance across different slip systems
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_2]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_3]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_4]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_5]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_6]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_7]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_8]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_9]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_10]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./slip_resistance_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./euler1]         # crystal orientations
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./euler2]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./euler3]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./residual_ssd_1] # AM-induced initial SSD density at the first slip system 
    order = FIRST
    family = MONOMIAL
  [../]
[]

[Functions]
  [./pre_residual] # to apply the residual deformation and initial dislocaiton densities within the first 1 second, and before applying the tensile force
    type = PiecewiseFunctions
    axis = t
    axis_coordinates = '1'
    functions = 't 1'
  [../]
  [./tension] # function to apply the tensile force on the RVE
    type = PiecewiseFunctions
    axis = t
    axis_coordinates = '1'
    functions = '0 0.002*${z_length}*t-0.002*${z_length}'
  [../]
  [./global_strain] # function to extract the global strain within gauge length
    type = ParsedFunction
    expression =(a-b)/${z_length}
    symbol_names = 'a b'
    symbol_values = 'w_front w_back'
  [../]
  [./hold] # function used on the fixed boundary conditions 
    type = ParsedFunction
    expression = 0
  [../]
[]

[Modules/TensorMechanics/Master/all]
  strain = FINITE
  add_variables = true
  generate_output = 'stress_xx stress_xy stress_yy strain_xx strain_xy strain_zz strain_xz strain_yz'
[]

[AuxKernels] # to link the [AuxVariables] with material properties in source code
  [plane_xy_dic]
    type = MaterialRealAux
    variable = plane_xy_dic
    property = plane_xy_dic
    execute_on = timestep_end
  []
  [plane_yy_dic]
    type = MaterialRealAux
    variable = plane_yy_dic
    property = plane_yy_dic
    execute_on = timestep_end
  []
  [epsilon_zz_dic]
    type = MaterialRealAux
    variable = epsilon_zz_dic
    property = epsilon_zz_dic
    execute_on = timestep_end
  []
  [copy_plane_yy]
    type = CopyValueAux
    variable = copy_plane_yy
    source = plane_yy
  []
  [copy_plane_xy]
    type = CopyValueAux
    variable = copy_plane_xy
    source = plane_xy
  []
  [copy_epsilon_zz]
    type = CopyValueAux
    variable = copy_epsilon_zz
    source = epsilon_zz
  []
  [copy_back]
    type = CopyValueAux
    variable = copy_back
    source = uz
    boundary = back
  []
  [copy_front]
    type = CopyValueAux
    variable = copy_front
    source = uz
    boundary = front
  []
  [temperature]
    type = FunctionAux
    variable = temperature
    function = pre_residual
    execute_on = timestep_begin
  []
  [./plane_yy]
    type = MaterialRealAux
    variable = plane_yy
    property = 110_yy
    execute_on = timestep_end
  [../]
  [./plane_xy]
    type = MaterialRealAux
    variable = plane_xy
    property = 110_xy
    execute_on = timestep_end
  [../]
  [./crss_or]
    type = MaterialStdVectorAux
    variable = crss_or
    property = crss_or
    index = 1
    execute_on = timestep_end
  [../]
  [./crss_gp_shear]
    type = MaterialStdVectorAux
    variable = crss_gp_shear
    property = crss_gp_shear
    index = 1
    execute_on = timestep_end
  [../]
  [./ave_damage]
    type = MaterialRealAux
    variable = ave_damage
    property = ave_damage
    execute_on = timestep_end
  [../]
  [./SB_initiation_strain]
    type = MaterialRealAux
    variable = SB_initiation_strain
    property = SB_initiation_strain
    execute_on = timestep_end
  [../]
  [./inside_sb_region]
    type = MaterialRealAux
    variable =inside_sb_region
    property = inside_sb_region
  [../]
  [./crss_solute]
    type = MaterialStdVectorAux
    variable = crss_solute
    property = crss_solute
    index = 1
    execute_on = timestep_end
  [../]
  [./epsilon_zz]
    type = RankTwoAux
    variable = epsilon_zz
    rank_two_tensor = epsilon_total #total_lagrangian_strain
    index_j = 2
    index_i = 2
    execute_on = timestep_end
  [../]
  [./epsilon_p_zz]
    type = RankTwoAux
    variable = epsilon_p_zz
    rank_two_tensor = epsilon_p
    index_j = 2
    index_i = 2
    execute_on = timestep_end
  [../]
  [./epsilon_e_zz]
    type = RankTwoAux
    variable = epsilon_e_zz
    rank_two_tensor = epsilon_e
    index_j = 2
    index_i = 2
    execute_on = timestep_end
  [../]
  [./stress_zz]
    type = RankTwoAux
    variable = stress_zz
    rank_two_tensor = stress
    index_j = 2
    index_i = 2
    execute_on = timestep_end
  [../]
  [./slip_resistance_gamma_prime]
    type = MaterialStdVectorAux
    variable = slip_resistance_gamma_prime
    property = slip_resistance_gamma_prime
    index = 1
    execute_on = timestep_end
  [../]
  [./slip_resistance_gamma]
    type = MaterialStdVectorAux
    variable = slip_resistance_gamma
    property = slip_resistance_gamma
    index = 1
    execute_on = timestep_end
  [../]
  [./applied_shear_stress]
    type = MaterialStdVectorAux
    variable = applied_shear_stress
    property = applied_shear_stress
    index = 1
    execute_on = timestep_end
  [../]
  [./ratio]
    type = MaterialStdVectorAux
    variable = ratio
    property = ratio
    index = 1
    execute_on = timestep_end
  [../]
  [./DD]
    type = MaterialRealAux
    variable = DD
    property = DD
    execute_on = timestep_end
  [../]
  [./SB_initiation]
    type = MaterialRealAux
    variable = SB_initiation
    property = SB_initiation
    execute_on = timestep_end
  [../]
  [./SB_detection_stress]
    type = SB_detection
    type0 = 0
    variable = ini_stress
    sb_initiation_old = sb_initiation_old
    stress_zz_old = global_stress
    strain_zz_old = strain_zz_old
    execute_on = timestep_end
  [../]
  [./SB_detection_strain]
    type = SB_detection
    type0 = 1
    variable = ini_strain
    sb_initiation_old = sb_initiation_old
    stress_zz_old = global_stress
    strain_zz_old = strain_zz_old
    execute_on = timestep_end
  [../]
  [./residual_eigenstrain]
    type = RankTwoAux
    variable = residual_eigenstrain
    rank_two_tensor = residual_eigenstrain
    index_j = 2
    index_i = 2
    execute_on = timestep_end
  [../]
  [./slip_increment_1]
   type = MaterialStdVectorAux
   variable = slip_increment_1
   property = slip_increment
   index = 0
   execute_on = timestep_end
  [../]
  [./slip_increment_2]
   type = MaterialStdVectorAux
   variable = slip_increment_2
   property = slip_increment
   index = 1
   execute_on = timestep_end
  [../]
  [./slip_increment_3]
   type = MaterialStdVectorAux
   variable = slip_increment_3   
   property = slip_increment
   index = 2
   execute_on = timestep_end
  [../]
  [./slip_increment_4]
   type = MaterialStdVectorAux
   variable = slip_increment_4
   property = slip_increment
   index = 3
   execute_on = timestep_end
  [../]
  [./slip_increment_5]
   type = MaterialStdVectorAux
   variable = slip_increment_5
   property = slip_increment
   index = 4
   execute_on = timestep_end
  [../]
  [./slip_increment_6]
   type = MaterialStdVectorAux
   variable = slip_increment_6
   property = slip_increment
   index = 5
   execute_on = timestep_end
  [../]
  [./slip_increment_7]
   type = MaterialStdVectorAux
   variable = slip_increment_7   
   property = slip_increment
   index = 6
   execute_on = timestep_end
  [../]
  [./slip_increment_8]
   type = MaterialStdVectorAux
   variable = slip_increment_8
   property = slip_increment
   index = 7
   execute_on = timestep_end
  [../]
  [./slip_increment_9]
   type = MaterialStdVectorAux
   variable = slip_increment_9
   property = slip_increment
   index = 8
   execute_on = timestep_end
  [../]
  [./slip_increment_10]
   type = MaterialStdVectorAux
   variable = slip_increment_10
   property = slip_increment
   index = 9
   execute_on = timestep_end
  [../]
  [./slip_increment_11]
   type = MaterialStdVectorAux
   variable = slip_increment_11   
   property = slip_increment
   index = 10
   execute_on = timestep_end
  [../]
  [./slip_increment_12]
   type = MaterialStdVectorAux
   variable = slip_increment_12
   property = slip_increment
   index = 11
   execute_on = timestep_end
  [../]
  [./build_slip_increment_vector]
    type = BuildArrayVariableAux
    variable = slip_increment_vector
    component_variables = 'slip_increment_1 slip_increment_2 slip_increment_3 slip_increment_4 slip_increment_5 slip_increment_6 slip_increment_7 slip_increment_8 slip_increment_9 slip_increment_10 slip_increment_11 slip_increment_12'
  [../]
  [./edge_directional_derivative]
    type = ArrayDirectionalDerivative
    variable = dslip_increment_dedge
    gradient_variable = slip_increment_vector
    dislo_character = edge
  	execute_on = timestep_end
  [../]
  [./screw_directional_derivative]
    type = ArrayDirectionalDerivative
    variable = dslip_increment_dscrew
    gradient_variable = slip_increment_vector
    dislo_character = screw
  	execute_on = timestep_end
  [../]  
  [./rho_ssd_1]
    type = MaterialStdVectorAux
    variable = rho_ssd_1
    property = rho_ssd
    index = 0
    execute_on = timestep_end
  [../]
  [./rho_ssd_2]
    type = MaterialStdVectorAux
    variable = rho_ssd_2
    property = rho_ssd
    index = 1
    execute_on = timestep_end
  [../]
  [./rho_ssd_3]
    type = MaterialStdVectorAux
    variable = rho_ssd_3
    property = rho_ssd
    index = 2
    execute_on = timestep_end
  [../]
  [./rho_ssd_4]
    type = MaterialStdVectorAux
    variable = rho_ssd_4
    property = rho_ssd
    index = 3
    execute_on = timestep_end
  [../]
  [./rho_ssd_5]
    type = MaterialStdVectorAux
    variable = rho_ssd_5
    property = rho_ssd
    index = 4
    execute_on = timestep_end
  [../]
  [./rho_ssd_6]
    type = MaterialStdVectorAux
    variable = rho_ssd_6
    property = rho_ssd
    index = 5
    execute_on = timestep_end
  [../]
  [./rho_ssd_7]
    type = MaterialStdVectorAux
    variable = rho_ssd_7
    property = rho_ssd
    index = 6
    execute_on = timestep_end
  [../]
  [./rho_ssd_8]
    type = MaterialStdVectorAux
    variable = rho_ssd_8
    property = rho_ssd
    index = 7
    execute_on = timestep_end
  [../]
  [./rho_ssd_9]
    type = MaterialStdVectorAux
    variable = rho_ssd_9
    property = rho_ssd
    index = 8
    execute_on = timestep_end
  [../]
  [./rho_ssd_10]
    type = MaterialStdVectorAux
    variable = rho_ssd_10
    property = rho_ssd
    index = 9
    execute_on = timestep_end
  [../]
  [./rho_ssd_11]
    type = MaterialStdVectorAux
    variable = rho_ssd_11
    property = rho_ssd
    index = 10
    execute_on = timestep_end
  [../]
  [./rho_ssd_12]
    type = MaterialStdVectorAux
    variable = rho_ssd_12
    property = rho_ssd
    index = 11
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_1]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_1
    property = rho_gnd_edge
    index = 0
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_2]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_2
    property = rho_gnd_edge
    index = 1
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_3]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_3
    property = rho_gnd_edge
    index = 2
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_4]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_4
    property = rho_gnd_edge
    index = 3
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_5]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_5
    property = rho_gnd_edge
    index = 4
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_6]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_6
    property = rho_gnd_edge
    index = 5
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_7]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_7
    property = rho_gnd_edge
    index = 6
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_8]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_8
    property = rho_gnd_edge
    index = 7
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_9]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_9
    property = rho_gnd_edge
    index = 8
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_10]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_10
    property = rho_gnd_edge
    index = 9
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_11]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_11
    property = rho_gnd_edge
    index = 10
    execute_on = timestep_end
  [../]
  [./rho_gnd_edge_12]
    type = MaterialStdVectorAux
    variable = rho_gnd_edge_12
    property = rho_gnd_edge
    index = 11
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_1]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_1
    property = rho_gnd_screw
    index = 0
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_2]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_2
    property = rho_gnd_screw
    index = 1
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_3]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_3
    property = rho_gnd_screw
    index = 2
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_4]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_4
    property = rho_gnd_screw
    index = 3
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_5]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_5
    property = rho_gnd_screw
    index = 4
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_6]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_6
    property = rho_gnd_screw
    index = 5
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_7]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_7
    property = rho_gnd_screw
    index = 6
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_8]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_8
    property = rho_gnd_screw
    index = 7
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_9]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_9
    property = rho_gnd_screw
    index = 8
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_10]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_10
    property = rho_gnd_screw
    index = 9
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_11]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_11
    property = rho_gnd_screw
    index = 10
    execute_on = timestep_end
  [../]
  [./rho_gnd_screw_12]
    type = MaterialStdVectorAux
    variable = rho_gnd_screw_12
    property = rho_gnd_screw
    index = 11
    execute_on = timestep_end
  [../]
  [./slip_resistance_1]
    type = MaterialStdVectorAux
    variable = slip_resistance_1
    property = slip_resistance
    index = 0
    execute_on = timestep_end
  [../]
  [./slip_resistance_2]
    type = MaterialStdVectorAux
    variable = slip_resistance_2
    property = slip_resistance
    index = 1
    execute_on = timestep_end
  [../]
  [./slip_resistance_3]
    type = MaterialStdVectorAux
    variable = slip_resistance_3
    property = slip_resistance
    index = 2
    execute_on = timestep_end
  [../]
  [./slip_resistance_4]
    type = MaterialStdVectorAux
    variable = slip_resistance_4
    property = slip_resistance
    index = 3
    execute_on = timestep_end
  [../]
  [./slip_resistance_5]
    type = MaterialStdVectorAux
    variable = slip_resistance_5
    property = slip_resistance
    index = 4
    execute_on = timestep_end
  [../]
  [./slip_resistance_6]
    type = MaterialStdVectorAux
    variable = slip_resistance_6
    property = slip_resistance
    index = 5
    execute_on = timestep_end
  [../]
  [./slip_resistance_7]
    type = MaterialStdVectorAux
    variable = slip_resistance_7
    property = slip_resistance
    index = 6
    execute_on = timestep_end
  [../]
  [./slip_resistance_8]
    type = MaterialStdVectorAux
    variable = slip_resistance_8
    property = slip_resistance
    index = 7
    execute_on = timestep_end
  [../]
  [./slip_resistance_9]
    type = MaterialStdVectorAux
    variable = slip_resistance_9
    property = slip_resistance
    index = 8
    execute_on = timestep_end
  [../]
  [./slip_resistance_10]
    type = MaterialStdVectorAux
    variable = slip_resistance_10
    property = slip_resistance
    index = 9
    execute_on = timestep_end
  [../]
  [./slip_resistance_11]
    type = MaterialStdVectorAux
    variable = slip_resistance_11
    property = slip_resistance
    index = 10
    execute_on = timestep_end
  [../]
  [./slip_resistance_12]
    type = MaterialStdVectorAux
    variable = slip_resistance_12
    property = slip_resistance
    index = 11
    execute_on = timestep_end
  [../]
  [./euler1]
    type = MaterialRealVectorValueAux
    variable = euler1
    property = Euler_angles
    component = 0
    execute_on = timestep_end
  [../]
  [./euler2]
    type = MaterialRealVectorValueAux
    variable = euler2
    property = Euler_angles
    component = 1
    execute_on = timestep_end
  [../]
  [./euler3]
    type = MaterialRealVectorValueAux
    variable = euler3
    property = Euler_angles
    component = 2
    execute_on = timestep_end
  [../]
  [./residual_ssd_1]
    type = MaterialStdVectorAux
    variable = residual_ssd_1
    property = residual_ssd
    index = 0
    execute_on = timestep_end
  [../]
[]

[BCs] # apply the boundary conditions
  [./symmx]
    type = CoupledVarDirichletBC 
    variable = uz
    boundary = 'front'
    v = copy_front
    function = tension
  [../]
  [./symmx2]
    type = CoupledVarDirichletBC 
    variable = uz
    boundary = 'back'
    v = copy_back
    function = hold
 [../]
[]

[Controls]
  [period0]
    type = TimePeriod
    disable_objects = 'BoundaryCondition::symmx BoundaryCondition::symmx2'
    start_time = '0'
    end_time = '1'
    execute_on = 'initial timestep_begin'
  []
  [period2]
    type = TimePeriod
    enable_objects = 'AuxKernels::copy_back AuxKernels::copy_front AuxKernels::copy_epsilon_zz AuxKernels::copy_plane_yy AuxKernels::copy_plane_xy'
    start_time = '0'
    end_time = '1'
    execute_on = 'initial timestep_begin'
  []
[] 

[Materials]
  [./elasticity_tensor] # define the elastic properties
    type = ComputeElasticityTensorCP
    C_ijkl = '1.35e5 0.96e5 0.96e5 1.35e5 0.96e5 1.35e5 0.65e5 0.65e5 0.65e5'
    fill_method = symmetric9
  [../]
  [./stress] # formation of stress in simulation
    type = ComputeDislocationCrystalPlasticityStress
    crystal_plasticity_models = 'trial_xtalpl'
    eigenstrain_names = residual_eigenstrain
    tan_mod_type = exact
  [../]
  [residual_eigenstrain] # load the residual deformation as the thermal expansion distribution across inter-dendrite to dendrite core
    type = ComputeCrystalPlasticityResidualEigenstrain
    eigenstrain_name = residual_eigenstrain
    deformation_gradient_name = thermal_deformation_gradient
    temperature = temperature
    uniform = 0
    residual_expansion_coefficients = '0 0 1E-2'
  []
  [./trial_xtalpl] # define the plastic properties based on the crystal plasticity model
    type = CrystalPlasticityDislocationDendrite
    number_slip_systems = 12
    slip_sys_file_name = input_slip_sys.txt # load the slip system of FCC
     
    # parameters in the crystal-plasticity model
  	ao =  0.05E-4 # reference slip rate
  	xm = 0.1  # strain rate sensitivity
  	alpha_0 = 0.1 # pre-factor of Taylor hardening law
  	y_c = 0.02 # critical annihilation distance
  	k_0 = 0.35 # coefficient representing the accumulation rate for SSD
    tau_c_0 = 12 # Peierls-Nabarro stress
    Wcr = 400 # critical dissipation energy
    Dcr = 0.8 # threshold damage for the failure
    shear_modulus = 35000 # shear_modulus used in Orowan stress
    burgers_vector_mag = 0.000255 # magnitude of Burgers vector

    # Intensities of TGMs
    inhomo_or = 1 # enable (=1) and disable (=0) TGMs type 3
    inhomo_shear = 1 # enable (=1) and disable (=0) TGMs type 2
    inhomo_solute = 1 # enable (=1) and disable (=0) TGMs type 4 
       
    k_solute = 1 # relative intensity of TGMs type 4 
    k_HL = 1 # relative intensity of TGMs type 2
    k_w = 1 # relative intensity of TGMs type 3
     
    with_GND = 1 # enable (=1) and disable (=0) GNDs during the deformation
    with_residual_dis = 1 # enable (=1) and disable (=0) residual SSD density
    
    # slip band nucleation and propagation parameters
    gamma_APB = 0.055 # anti-phase boundary energy, used in the criterion of slip band nucleation
    sb_evo_rate = 0 # slip band propagation rate
    initial_SB_width = 0.4 # initial slip band width

    # to link with Postprocessors for slip band simulation
    z_length=${z_length}
    D1_old=D1_old
    D2_old=D2_old 
    ini_stress = ini_stress
    ini_strain = ini_strain
    stress_zz_old = global_stress
    sb_initiation_old = sb_initiation_old
    min_crss_shear = min_crss_shear
    max_crss_shear = max_crss_shear
    strain_zz_old = global_strain
    
    # read initial concentration distributions of solute and initial gamma/gamma prime morphology
    read_conc_ele = init_conc_ele_read
    read_micro_morph = init_micro_morph_read
    
    # GND calculation 
  	dslip_increment_dedge = dslip_increment_dedge
  	dslip_increment_dscrew = dslip_increment_dscrew
  	print_state_variable_convergence_error_messages = true
  [../]
  
  # convert the coordinate of strain within RVE onto the observation plane in DIC, for the comparison between CPFE and DIC results
  [./epsilon_zz_dic] 
    type = ParsedMaterial
    property_name = epsilon_zz_dic
    coupled_variables = 'epsilon_zz copy_epsilon_zz'
    expression = 'epsilon_zz - copy_epsilon_zz'
  [../]
  [./plane_yy_dic]
    type = ParsedMaterial
    property_name = plane_yy_dic
    coupled_variables = 'plane_yy copy_plane_yy'
    expression = 'plane_yy - copy_plane_yy'
  [../]
  [./plane_xy_dic]
    type = ParsedMaterial
    property_name = plane_xy_dic
    coupled_variables = 'plane_xy copy_plane_xy'
    expression = 'plane_xy - copy_plane_xy'
  [../]
[]

[Postprocessors] # to be extracted into the .csv file
  [./slip_resistance_gamma]
    type = ElementAverageValue
    variable = slip_resistance_gamma
    block = 2
  [../]
  [./slip_resistance_1]
    type = ElementAverageValue
    variable = slip_resistance_1
    block = 2
  [../]
  [./crss_or]
    type = ElementAverageValue
    variable = crss_or
    block = 2
  [../]
  [./SB_initiation_strain]
    type = ElementAverageValue
    block = 2
    variable =SB_initiation_strain
  [../]
  [./inside_sb_region]
    type = ElementAverageValue
    variable = inside_sb_region
    block = 2
  [../]
  [./strain_zz_old]
    type = PointValue
    variable = epsilon_p_zz
    point = '0 0 0'
    execute_on = timestep_end
  [../]
  [./epsilon_p_zz]
    type = ElementAverageValue
    variable = epsilon_p_zz
    block = 2
  [../]
  [./D1_old] # used to calculate slip band propagation
    type = ElementExtremeValue
    variable = DD
    value_type = min
    block = 2
  [../]
  [./D2_old] # used to calculate slip band propagation
    type = ElementExtremeValue
    variable = DD
    value_type = max
    block = 2
  [../]
  [./sb_initiation_old] # used to calculate slip band nucleation
    type = ElementExtremeValue
    variable = SB_initiation
    value_type = max
    block = 2
  [../]
  [./local_strain_interdendrite]
    type = PointValue
    variable = epsilon_zz_dic
    point = '0 0 0'
    execute_on = timestep_end
  [../]
  [./local_strain_dendrite_core]
    type = PointValue
    variable = epsilon_zz_dic
    point = '-1 1 -2' 
    execute_on = timestep_end
  [../]
  [./average_strain_inside_SB]
    type = FunctionValuePostprocessor
    function = average_strain
    execute_on = timestep_end
  [../]
  [./local_crss_interdendrite]
    type = PointValue
    variable = slip_resistance_gamma
    point = '0 0 0'
    execute_on = timestep_end
  [../]
  [./local_crss_dendrite_core]
    type = PointValue
    variable = slip_resistance_gamma
    point = '-1 1 -2' 
    execute_on = timestep_end
  [../]
  [./min_crss_shear]
    type = ElementExtremeValue
    variable = crss_gp_shear
    value_type = min
    block = 2
  [../]
  [./crss_gp_shear]
    type = ElementExtremeValue
    variable = crss_gp_shear
    value_type = max
    block = 2
  [../]
  [./max_crss_shear]
    type = ElementExtremeValue
    variable = crss_gp_shear
    value_type = max
    block = 2
  [../]
  [./residual_ssd_1]
    type = ElementAverageValue
    variable = residual_ssd_1
    block = 2
    execute_on = timestep_end
  [../]
  [./local_damage_dendrite_core]
     type = PointValue
     variable = ave_damage
     point = '-1 1 -2'
     execute_on = timestep_end
  [../]
  [./local_damage_interdendrite]
     type = PointValue
     variable = ave_damage
     point = '0 0 0'
     execute_on = timestep_end
  [../]
  [./global_strain]  # the engineering strain used in stress-strain curves
    type = FunctionValuePostprocessor
    function = global_strain
    execute_on = timestep_end
  [../]
  [./w_front]
    type = SideAverageValue
    variable = uz
    boundary= 'front1'
  [../]
  [./w_back]
    type = SideAverageValue
    variable = uz
    boundary= 'back1'
  [../]
  [./global_stress]  # the engineering stress used in stress-strain curves
    type = SideAverageValue
    variable = stress_zz
    boundary= 'front'
  [../]
[]

[Functions] # Functions used in Postprocessors
  [average_strain]
    type = ParsedFunction
    expression = a*0.5+b*0.5
    symbol_names = 'a b'
    symbol_values = 'local_strain_interdendrite local_strain_dendrite_core'
  []
[]

[UserObjects]
  [./init_conc_ele_read] # atomic percentage of solid-solution elements, Al/Co/Cr/Mo/Ti in order
    type = PropertyReadFile
	  prop_file_name = 'conc_ele.txt'
	  nprop = 5
    read_type = element
  [../]
  [./init_micro_morph_read] # morphology of gamma / gamma prime phases, H/L/wH/wL/fr in order
    type = PropertyReadFile
	  prop_file_name = 'micro_morph.txt'
	  nprop = 5
    read_type = element
  [../]
[]

[Preconditioning]
  [./smp]
    type = SMP
    full = true
  [../]
[]

[Executioner] # setup for the solver of FEM
  type = Transient
  solve_type = 'NEWTON'
  petsc_options = '-snes_ksp_ew'
  petsc_options_iname = '-pc_type -pc_hypre_type -ksp_gmres_restart'
  petsc_options_value =   'hypre    boomeramg          30'
  nl_rel_tol = 2E-6
  nl_abs_tol = 2E-6
  line_search = 'none'
  automatic_scaling = true
  [./TimeStepper]
    type = IterationAdaptiveDT
    dt = 0.1
    growth_factor = 1.1
    cutback_factor_at_failure= 0.5
  [../]
  end_time = 800
  dtmin = 5E-20
  dtmax = 10
[]

[Outputs] # simulation outputs
  print_linear_residuals =false
  [./case_980_1]
    type = CSV
  [../]
  [./case_980_2]
    type = Exodus
    interval = 20
  [../]
  [./case_980_3]
    type = Checkpoint
    time_step_interval = 10000
    num_files = 1
  []
[]
