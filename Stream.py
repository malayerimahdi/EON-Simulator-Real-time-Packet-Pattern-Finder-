import time
import re
import pandas as pd
import numpy as np

def follow(inputFile):
    # inputFile.seek(0, 2)
    
    while True:
        line = inputFile.readline()
        
        if not line:
            time.sleep(0.1)
            line = inputFile.readline()
            continue
        
        yield line

if __name__ == '__main__':
    filepath = "cons22.txt" # input file
    inputFile = open(filepath, 'r')
    df = pd.DataFrame(columns=['simtime', 'source', 'dest', 'needed', 'holding time']) # creating data frame
    df_simtime = pd.DataFrame(columns=['source', 'dest', 'needed', 'simtime_average'])
    
    times = []
    
    while True:
        t = input("Enter a new time for averaging: ")
        
        if t == '00':
            break
        
        times.append(t)
    
    df_needed = pd.DataFrame(columns=['source', 'dest']+times)
    
    print("\n--> Starting the main process")
    cnt = 0
    lines = follow(inputFile)
    for line in lines:
        #print(line)
        result = re.search(" SIMTIME: (.*)   source:(.*) and Dest:(.*) and needed:(.*) and holding time:(.*)", line) # extracting data from each line
        cnt += 1
        print("Reading Line {}".format(cnt))
        
        if result: # if line is not empty
           simtime = float(result.group(1))
           source = int(result.group(2))
           dest = int(result.group(3))
           needed = int(result.group(4))
           holding_time = float(result.group(5))
           
           # adding data to data frame
           df = df.append({'simtime': simtime, 'source': source, 'dest': dest, 'needed': needed, 'holding time': holding_time}, ignore_index=True)
           
           df_select = df.loc[(df['source'] == source) & (df['dest'] == dest)] # selecting rows of the main data frame for the specified pair
           
           # Duplicate File
           duplicateRows = df_select.loc[(df_select['needed'] == needed)]
           
           if duplicateRows.shape[0] > 1:
               duplicateRows = duplicateRows.sort_values(by=['simtime'])
               diff_simtime = list(duplicateRows.diff(axis=0, periods=1)['simtime'])
               # diff_simtime[0] = duplicateRows.iloc[0, 0]
               diff_simtime = [x for x in diff_simtime if ~np.isnan(x)]
               diff_simtime = [round(x, 1) for x in diff_simtime]
               # print(diff_simtime)
               if len(diff_simtime) > 1 and len(set(diff_simtime)) == 1:
                   equal_diff = diff_simtime[0]
                   duplicateRows = duplicateRows.append({'simtime': equal_diff}, ignore_index=True)
                   duplicatepath = str(source) + "--" + str(dest) + "_" + str(needed) + "_true.csv"
               else:
                   duplicatepath = str(source) + "--" + str(dest) + "_" + str(needed) + "_false.csv"
               
               duplicateRows.to_csv(duplicatepath, index=False)
               
               # if ((df_simtime['source'] == source) & (df_simtime['dest'] == dest) & (df_simtime['needed'])).any():
               #     df_simtime.loc[(df_simtime['source'] == source) & (df_simtime['dest'] == dest) & (df_simtime['needed']), 'simtime_average'] = diff_average
               # else:
               #     df_simtime = df_simtime.append({'source': source, 'dest': dest, 'needed': needed, 'simtime_average': diff_average}, ignore_index=True)
            
               # outputfile_simtime = "simtime_average.csv"
               # df_simtime.to_csv(outputfile_simtime, index=False)
                
           # Average File
           sum_needed = []
           average_needed = []
           
           for t in times:
               df_select2 = df_select.loc[df_select['simtime'] <= int(t)] # selecting rows of the selected data frame with 'simtime' less than or equal to (time = t)
               sum_needed.append(df_select2['needed'].sum())
               average_needed.append(df_select2['needed'].mean()) # calculating average of 'needed' for the selected rows
               
           df_average = pd.DataFrame({'Time (s)': times, 'Sum': sum_needed, 'Average': average_needed})
           
           # if ((df_needed['source'] == source) & (df_needed['dest'] == dest)).any():
           #     df_needed.loc[(df_needed['source'] == source) & (df_needed['dest'] == dest), times] = average_needed
           # else:
           #     df_needed = df_needed.append({'source': source, 'dest': dest}, ignore_index=True)
           #     df_needed.loc[(df_needed['source'] == source) & (df_needed['dest'] == dest), times] = average_needed
           
           outputfile_needed = str(source) + "--" + str(dest) + "_needed.csv"
           df_average.to_csv(outputfile_needed, index=False)
           
           # df_sort = df.sort_values(by=['source', 'dest', 'simtime']) # sorting data frame
           
           # Main Output File
           outputfilepath = str(source) + "--" + str(dest) + ".txt"
           of = open(outputfilepath, 'a')
            
           of.write("\n SIMTIME: %f   source:%d and Dest:%d and needed:%d and holding time:%f" % (simtime, source, dest, needed, holding_time))       
        
           of.close()