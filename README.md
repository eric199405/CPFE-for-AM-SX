# CPFE-for-AM-SX

CPFE code for AM SX, to simulate the deformation at dendrite scale, under the open-source MOOSE software.

The CPFE simulation is conducted under open-source MOOSE platform for the FEM analysis, with source code for the paper attached. We also provide a simulation case in ‘CPFE_980C.i’, corresponding to the simulation at 980 °C for the deformation around the dendrites.

Kindly note that the MOOSE software under Linux system is not available in the Code Ocean. To run the CPFE simulation in MOOSE software, please do:

Step 1: Download and install MOOSE in Linux system (https://mooseframework.inl.gov/getting_started/installation/);

Step 2: Install the APP named ‘am_sx_superalloy’ targeting the attached source code, with instructions in https://mooseframework.inl.gov/getting_started/new_users.html;

Step 3: Run the case of ‘CPFE_980C.i’ that we provided, by using the command: ./am_sx_superalloy-opt -i CPFE_980C.i

Step 4: View the CPFE simulation results using ParaView 5.12.
