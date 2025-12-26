This repository has been created for the following research purposes: 

Research Title: "The Homophily Effect: Strengthening or Limiting Organizational Learning" 

Description: 
This reserach aims to investigate how homophily in the organizational elarning process affects and influences organizational performance.
The research proposes an extended model of James G March's Model of Mutual Organizational learning with the addition of the social phenomena homophily. 

Files: 
*Input.dat 
- simulation parameters and setting for the simulation

*march_homophily_extension
- Code File : compile through gcc to a.out file
- Organization Learning paramter: h
  - learning from high performing individuals (0)
  - learning from homophilous performing individuals (0<h<1), where 1 = 100% homophilous
- Individual Learning parameter:
   - mutual learning from organization
   - homophilous learning from organization
 
*merge.sh
- compile merge all results into one file

*run.sh
- run merge and output results.txt 
