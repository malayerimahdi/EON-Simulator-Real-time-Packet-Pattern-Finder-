#include <iostream>
#include <fstream>
#include <string> 
#include <vector>

#include <windows.h>
#include <math.h> 

using namespace std;

struct packet {
    float simtime;
    short int source;
    short int dest;
    short int needed;
    float holding_time;
};

vector<packet> df_filter(vector<packet> const &df, short int source, short int dest) {
	
	vector<packet> v;
	
	for (auto &i : df) {
		if (i.source == source && i.dest == dest) {
			v.push_back(i);
		}
	}
	
	return v;
}

vector<packet> df_filter2(vector<packet> const &df, short int needed) {
	
	vector<packet> v;
	
	for (auto &i : df) {
		if (i.needed == needed) {
			v.push_back(i);
		}
	}
	
	return v;
}

bool cmpf(float a, float b, float epsilon = 0.1) {
	return (fabs(a - b) < epsilon);
}

float get_diff(vector<packet> const &df) {
	
	float diff = df.at(1).simtime - df.at(0).simtime;
	float diff2;
	
	for(vector<int>::size_type i = 2; i != df.size(); i++) {
		
		diff2 = df.at(i).simtime - df.at(i-1).simtime;
		
		if (!cmpf(diff2, diff)) {
			return -1.0;
		}
	}
	
	float value = (int)(diff * 10 + 0.5); 
    return (float)value / 10; 
}

int main() {
	
	vector<packet> df;
	vector<packet> df_select;
	vector<packet> duplicate;
	vector<string> t;
	
	float simtime_temp, holding_time_temp;
	short int source_temp, dest_temp, needed_temp;
	
	unsigned int simtime_pos, source_pos, dest_pos, needed_pos, holding_time_pos;
	
	string filePath = "cons22.txt";
	ifstream infile(filePath);
	
	ofstream outfile;
	string outputPath;
	
	int start_pos = 0;
	int end_pos = 0;
	
	long length;
	
	float diff;
	
	int sum, count;
	
	string temp;
	
	while (1) {
		
		cout << "Enter a new time for averaging: ";
		cin >> temp;
		
		if (temp == "00") {
			break;
		}
		
		t.push_back(temp);
	}
	
	cout << "\n--> Starting main process" << endl;

	int cnt = 0;

	while (1) {
		
		if (infile.is_open()) {
		
			string line;
		
			infile.seekg(0, infile.end);
            end_pos = infile.tellg();  
            infile.seekg(start_pos, infile.beg);
            
            length = end_pos - start_pos;
            
			while(getline(infile, line)) {
				
				cnt++;
				
				if (!line.empty()) {
						
					simtime_pos = line.find("SIMTIME");
					source_pos = line.find("source");
					dest_pos = line.find("Dest");
					needed_pos = line.find("needed");
					holding_time_pos = line.find("holding time");
					
					simtime_temp = stof(line.substr(simtime_pos + 9, source_pos - simtime_pos - 12));
					source_temp = stoi(line.substr(source_pos + 7, dest_pos - source_pos - 12));
					dest_temp = stoi(line.substr(dest_pos + 5, needed_pos - dest_pos - 10));
					needed_temp = stoi(line.substr(needed_pos + 7, holding_time_pos - needed_pos - 12));
					holding_time_temp = stof(line.substr(holding_time_pos + 13));
					
					cout << "Reading Line " << cnt << " --> Simulation Time = " << simtime_temp << endl;
					
					df.push_back({simtime_temp, source_temp, dest_temp, needed_temp, holding_time_temp});
					
					df_select = df_filter(df, source_temp, dest_temp);
					duplicate = df_filter2(df_select, needed_temp);
					
					if (duplicate.size() > 1) {
						
						diff = get_diff(duplicate);
						
						if (duplicate.size() > 2 && diff > 0.0 ) {
							
							outputPath = to_string(source_temp) + "--" + to_string(dest_temp) + "_" + to_string(needed_temp) + "_true.csv";
							outfile.open(outputPath);
							outfile << "simtime, source, dest, needed, holding time" << endl;
							
							for (auto &i : duplicate) {
								outfile << i.simtime << ", " << i.source << ", " << i.dest << ", " << i.needed << ", " << i.holding_time << endl;
							}
							
							outfile << diff;
							outfile.close();
							
						} else {
							
							outputPath = to_string(source_temp) + "--" + to_string(dest_temp) + "_" + to_string(needed_temp) + "_false.csv";
							outfile.open(outputPath);
							outfile << "simtime, source, dest, needed, holding time" << endl;
							
							for (auto &i : duplicate) {
								outfile << i.simtime << ", " << i.source << ", " << i.dest << ", " << i.needed << ", " << i.holding_time << endl;
							}
							
							outfile.close();
							
						}
					}
					
					outputPath = to_string(source_temp) + "--" + to_string(dest_temp) + "_needed.csv";
					outfile.open(outputPath);
					outfile << "Time (s), Sum, Count, Average" << endl;
					
					for (auto &time : t) {
								
						sum = 0;
						count = 0;
						
						for (auto &i : df_select) {
							if (i.simtime <= stof(time)) {
								sum += i.needed;
								count += 1;
							}
						}
						
						outfile << time << ", " << sum << ", " << count << ", " << (float)sum/count << endl;
					}
					
					outfile.close();
					
					outputPath = to_string(source_temp) + "--" + to_string(dest_temp) + ".txt";
					outfile.open(outputPath, ios_base::app);
					outfile << line << endl;
					outfile.close();
				
				}
				
			}
			
			infile.close();
			start_pos = end_pos;
				
		}
		
		Sleep(1);
		infile.open(filePath);  
	
	}

	return 0;
}
