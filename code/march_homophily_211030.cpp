

/****************************************************************************************
organization goal = individual goal
1. organization learns from superior individuals with p2.
2. Beta % of individuals learn from organization with p1 when organization is smarter while others learn when codes are different.
******************************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <iomanip>
#include <random>
using namespace std;

#define num_population 50 // number of total population
#define num_dimensions 30 // number of dimensions in knowledge set
#define num_steps 1000 //
#define num_sample 500  // number of simulation

/***************declare variables from input.dat*************/
double p1; // learning rate of individuals
double p2; // learning rate of organization
double beta; // number of dimensions required for homophily selection 
//int iteration;
/*************************************************************/

/************* declare variables for time series***************/
//vector<double> org_perf_time_series;
//vector<double> indv_perf_time_series;
//vector<double> zero_indv_time_series;
//vector<double> equilibrium_time_series;
/**************************************************************/



/******************** declare variables**********************/
int count_int; //number of iterations
ofstream out1("time.txt");
ofstream out("results.txt");
/**************************************************************/



/*****************declare variables for results****************/
double zero_indv, num_scale, perf_scale;
double avg_org_perf, avg_indv_perf, avg_equilibrium, avg_num_scale;
double final_avg_org_perf, final_avg_indv_perf, final_avg_equilibrium, final_zero_indv;
/**************************************************************/



/*********************declare classes***************************/
class individuals {
public:
	vector<int> indv_goal; // -1 or 1 (fixed)
	vector<int> indv_code; // -1, 0, or 1 (change by learning org.)

	int indv_perf; // org_goal == indv_code
	int indv_homophily; // org_code == indv_code // homophily count
};

class organization {
public:
	vector<int> m_goal; // -1 or 1 (fixed)
	vector<int> m_code; // -1, 0, or 1 (change by learning indv; start w/ all 0)

	int m_perf; // org_goal == org_code

	vector<int>selective_indv; // individuals who does selective learning
	vector<int>homophily_list; // list of homophily individuals
	vector<int>homophily_code; // knowledge set of homophily individuals	
	int num_homophily;
	
	int num_perf;
	vector<int>performance_list;
	vector<int>performance_code;
	
	vector<int>scale_list;
	vector<int>scale_code;
};
/****************************************************************/



/**************generating variables of class**********************/
individuals* agent = new individuals[num_population];
organization* org = new organization[1];
/*****************************************************************/



/**********************declare functions*************************/
void readDataFromFile();

void clear();

void org_initialize();
void indv_initialize();

void update_indv();
void update_org();

void get_homophily_list();
void update_homophily_code();

void get_perf_list();
void update_perf_code();

void get_scale_list();
void update_scale_code();

void learn_from_org();
void learn_from_indv();
void learn_from_indv_m();
void mutual_learn_from_org();
void learn_from_org_h();
void sum_output();
void update_time(int);
void output(int);
void indiv_code_time();
/*********************************************************************/


/*************************main function*******************************/
int main(int argc, char* argv[]) {

/******initializing MPI*******/

	readDataFromFile();
	clear();

	for (int i = 0; i < num_sample; i++) {
		org_initialize();
		indv_initialize();

		update_org();
		update_indv();

		for (int count_int = 0; count_int < num_steps; count_int++) {

			learn_from_indv();
			learn_from_org_h();

			update_org();
			update_indv();

			sum_output();
			update_time(count_int);
			indiv_code_time();
		} // end of iteration
		output(i);
	} // end of sample
	return 0;
}// end of main
 /*************************************************************************/


 /****************declare function***************/
void readDataFromFile() {
	string tmp;
	cin >> tmp >> p1;
	cin >> tmp >> p2;
	cin >> tmp >> beta;
}

void clear() {
	avg_equilibrium = 0;
	avg_org_perf = 0;
	avg_indv_perf = 0;
	
	final_avg_org_perf = 0;
	final_avg_indv_perf = 0;
	final_avg_equilibrium = 0;
	final_zero_indv = 0;
}

void org_initialize() {   //assigning values for organizational knowledge set
	org[0].m_code.clear();
	org[0].m_goal.clear();

	for (int i = 0; i < num_dimensions; i++) {
		org[0].m_goal.push_back(rand() % 2 * 2 - 1);  // -1 or 1
		org[0].m_code.push_back(0); // begin with all '0'
	}
}

void indv_initialize() { // assigning values for each indv's knowledge set
	org[0].selective_indv.clear();
	double temprand = (double)(rand()%10000)/10000;
	
	for (int j = 0; j < num_population; j++) {
		agent[j].indv_goal.clear();
		agent[j].indv_code.clear();


		for (int i = 0; i < num_dimensions; i++) {
			agent[j].indv_goal.push_back(org[0].m_goal[i]);// -1 or 1 (opposite of org_goal)}
			agent[j].indv_code.push_back(rand() % 3 - 1); // -1, 0, or 1
		}
	}
}

void update_org() {   // get organizational performance
	org[0].m_perf = 0;

	for (int i = 0; i < num_dimensions; i++) {
		if (org[0].m_goal[i] == org[0].m_code[i]) org[0].m_perf++;
	}
}

void update_indv() {  // get (1) individual performance, (2) individual fitness, (3) individual satisfaction, and (4) equilibrium knowledge level

	for (int j = 0; j < num_population; j++) {
		agent[j].indv_perf = 0;
		agent[j].indv_homophily = 0;

		for (int i = 0; i < num_dimensions; i++) {
			if (org[0].m_goal[i] == agent[j].indv_code[i]) agent[j].indv_perf++;
			if(agent[j].indv_code[i] == org[0].m_code[i]) agent[j].indv_homophily++;
		}

	}
}

void get_homophily_list() {    // seek indvs who performs similar to org
	org[0].homophily_list.clear();
	org[0].num_homophily = 0;
	double r = (double)(rand()%10000)/10000;
	for(int j = 0; j < num_population; ++j){
		double pick =(agent[j].indv_homophily/30);
		if(pick >= r){
			org[0].homophily_list.push_back(j);
			org[0].num_homophily++;
		}
	}
}


void update_homophily_code() {     // get the average code for homogeneous individuals
	org[0].homophily_code.clear();

	get_homophily_list();

	for (int i = 0; i < num_dimensions; i++) {
		int temp_code = 0;

		for (int j = 0; j < org[0].homophily_list.size(); j++) {
			temp_code += agent[org[0].homophily_list[j]].indv_code[i];
		}
		if (temp_code > 0) {
			org[0].homophily_code.push_back(1);
		}
		else if (temp_code == 0) {
			org[0].homophily_code.push_back(0);
		}
		else if (temp_code < 0) {
			org[0].homophily_code.push_back(-1);
		}
		else {
			throw temp_code;
		}
	}
}
void get_perf_list() {    // seek indvs who performs similar to org
	org[0].performance_list.clear();
	org[0].num_perf = 0;

	for (int j = 0; j < num_population; j++) {
		if (org[0].m_perf < agent[j].indv_perf) {
			org[0].performance_list.push_back(j);
			org[0].num_perf++;
			}
	}	
}

void update_perf_code() {     // get the average code for homogeneous individuals
	org[0].performance_code.clear();

	get_perf_list();

	for (int i = 0; i < num_dimensions; i++) {
		int temp_code = 0;

		for (int j = 0; j < org[0].performance_list.size(); j++) {
			temp_code += agent[org[0].performance_list[j]].indv_code[i];
		}
		if (temp_code > 0) {
			org[0].performance_code.push_back(1);
		}
		else if (temp_code == 0) {
			org[0].performance_code.push_back(0);
		}
		else if (temp_code < 0) {
			org[0].performance_code.push_back(-1);
		}
		else {
			throw temp_code;
		}
	}
}

void learn_from_indv() {  // organization learn from superior individuals
	update_homophily_code();
	update_perf_code();
	
	for (int i = 0; i < num_dimensions; i++) {
		double temprand = (double)(rand() % 10000)/10000;
		if (beta >= temprand){
			int k = 0;
			int temp_k = 0;
			for (int j = 0; j < org[0].num_homophily; j++) {
				if (agent[org[0].homophily_list[j]].indv_code[i] == org[0].m_code[i])
					temp_k++;
			}

			k = (org[0].num_homophily - temp_k) - temp_k;

			if (k>0){
				double temp_p2 = 1 - pow(1 - p2, k);

				if (org[0].homophily_code[i] != 0) {
					if (org[0].homophily_code[i] != org[0].m_code[i]) {
						if (temprand < temp_p2) {
							replace(org[0].m_code.begin(), org[0].m_code.end(), org[0].m_code[i], org[0].homophily_code[i]);
							num_scale++;
						}
					}
				}	
			}
		}
		else{
			int k = 0;
			int temp_k = 0;
			perf_scale++;
			for (int j = 0; j < org[0].num_perf; j++) {
				if (agent[org[0].performance_list[j]].indv_code[i] == org[0].m_code[i])
					temp_k++;
			}

			k = (org[0].num_perf - temp_k) - temp_k;

			if (k>0){
				double temp_p2 = 1 - pow(1 - p2, k);

				if (org[0].performance_code[i] != 0) {
					if (org[0].performance_code[i] != org[0].m_code[i]) {
						if (temprand < temp_p2) {
							replace(org[0].m_code.begin(), org[0].m_code.end(), org[0].m_code[i], org[0].performance_code[i]);
						
						}	
					
					}	
				}
			}	
	
		}
//	string msg = "number of homophily";
//	cout << msg << "\t" << num_scale << endl;
//	string msg2  =  "number of superiors";
//	cout << msg2 << "\t" << org[0].num_perf<< endl;
	}

}

void learn_from_org() { // individual learns when individual code is different from organizational code and organizational code is not 0
	for (int j = 0; j < num_population; j++) {
		if(agent[j].indv_perf < org[0].m_perf) {
			for (int i = 0; i < num_dimensions; i++) {
				double temprand2 = (double)(rand() % 1000) / 1000;
				if (org[0].m_code[i] != 0) {
					if (agent[j].indv_code[i] != org[0].m_code[i]) {
						if (temprand2 <= p1) {
							replace(agent[j].indv_code.begin(), agent[j].indv_code.end(), agent[j].indv_code[i], org[0].m_code[i]);
						}	
					}				
				}
			}
		}
	}
}

void learn_from_org_h() { // individual learns when individual code is different from organizational code and organizational code is not 0
	for (int j = 0; j < num_population; j++) {
		double r = (double)(rand() % 100)/ 100;
		double pick = (double)agent[j].indv_homophily/30.0;
		if (pick > r){	
			for (int i = 0; i < num_dimensions; i++) {
				double temprand2 = (double)(rand() % 1000) / 1000;
				if (org[0].m_code[i] != 0) {
					if (agent[j].indv_code[i] != org[0].m_code[i]) {
						if (temprand2 <= p1) {
							replace(agent[j].indv_code.begin(), agent[j].indv_code.end(), agent[j].indv_code[i], org[0].m_code[i]);
						}
					}
				}				
			}
		}
	}
}
void mutual_learn_from_org(){
	for (int j = 0;j < num_population; j++){
		for (int i = 0; i < num_dimensions; i++) {
			double temprand = (double)(rand() % 1000) / 1000;
			if (org[0].m_code[i] != 0) {
				if (agent[j].indv_code[i] != org[0].m_code[i]) {
					if (temprand <= p1) {
						replace(agent[j].indv_code.begin(), agent[j].indv_code.end(), agent[j].indv_code[i], org[0].m_code[i]);
					}	
				}
			}
		}
	}
}

void sum_output() {
zero_indv = 0;
double temp_avg_equilibrium = 0;
double temp_avg_org_perf = 0;
double temp_avg_indv_perf = 0;

	for (int j = 0; j < num_population; j++) {
		temp_avg_equilibrium += (double)agent[j].indv_homophily / num_dimensions;
		temp_avg_org_perf += (double)org[0].m_perf / num_dimensions;
		temp_avg_indv_perf += (double)agent[j].indv_perf / num_dimensions;
			if (find(agent[j].indv_code.begin(), agent[j].indv_code.end(), 0) != agent[j].indv_code.end()) {
				zero_indv++;
			}
	}
	avg_equilibrium = temp_avg_equilibrium / num_population;
	avg_org_perf = temp_avg_org_perf / num_population;
	avg_indv_perf = temp_avg_indv_perf / num_population;
}

void update_time(int temp_t){
	if ((temp_t%10) == 0){
		out1 << temp_t << "\t" << p1 << "\t" << p2 << "\t" << beta << "\t" << zero_indv << "\t" << avg_equilibrium << "\t" << avg_org_perf << "\t" << avg_indv_perf << "\t" << endl;
	}
}

void indiv_code_time() {
	for(int j = 0; j <= num_population; j++){
		out << indv_code[j] << ' ' 
	}
}

void output(int temp_s) {//output
//ofstream out("results.txt");

	final_zero_indv += zero_indv / num_sample;
	final_avg_equilibrium += avg_equilibrium / num_sample;
	final_avg_org_perf += avg_org_perf / num_sample;
	final_avg_indv_perf += avg_indv_perf / num_sample;

	if (temp_s == num_sample - 1) {
		out << p1 << "\t" << p2 << "\t" << beta << "\t" << final_zero_indv << "\t" << final_avg_equilibrium << "\t" << final_avg_org_perf << "\t" << final_avg_indv_perf << "\t" << avg_num_scale << "\t" << endl;
	}
}
			/*******:wq*****************************************/

