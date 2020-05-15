import matplotlib.pyplot as plt
import time as tm
import os
import numpy as np
import copy


test_list = ["01-main", "02-switch", "11-join", "12-join-main", "21-create-many", "22-create-many-recursive", "23-create-many-once", "31-switch-many", "32-switch-many-join", "51-fibonacci"]
arg_list = [(4,1), (5,1), (6,1), (7,2), (8,2), (9,2)] #tests which need n arguments to be given !!! THIS LIST IS SORTED FOR PERFORMANCES !!!
arg = []
for i in range(1,1001):
    arg.append(i)
arg_test = 5
directory = "./install/bin/"

def print_graph_test():
    suffix = "_pthread"
    length = len(test_list)
    res = []
    res_pthread = []
    maxi = 0
    arg_pointer = 0
    
    for i in range(length):
        argument = " "

        if arg_list[arg_pointer][0] == i:
            for j in range(arg_list[arg_pointer][1]):
                argument += str(arg[1000]) + " "
            arg_pointer += 1
            
                
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC)
        os.system(directory + test_list[i] + argument)
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC) - tempo
        res += [tempo]
        if tempo > maxi:
            maxi = tempo
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC)
        os.system(directory + test_list[i] + suffix + argument)
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC) - tempo
        res_pthread += [tempo]
        if tempo > maxi:
            maxi = tempo
    width_ = 0.6
    plt.bar(range(length), res, width = width_, color = 'yellow', edgecolor = 'blue', linewidth = 2)
    plt.bar([x + width_ for x in range(length)], res_pthread, width = width_, color = 'red', edgecolor = 'blue', linewidth = 2)
    plt.show()
    
def print_graph_thread():
    length = len(arg)
    res = []
    
    for i in range(length):
        argument = " " + str(i) + " "
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC)
        os.system(directory + test_list[arg_test] + argument)
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC) - tempo
        res += [tempo]
   # final_res = copy.deepcopy(res)
   # for i in range(10,len(res)-10):
   #     value = 0
   #     for j in range(20):
   #         value += res[i-10+j]
   #     final_res[i] = value/20
    final_res = []
    for i in range(0,1000,20):
        value = 0
        for j in range(20):
            value += res[i + j]
        final_res += [value]
    plt.plot(final_res)
    plt.show()
    
def print_graph_pthread():
    length = len(arg)
    res = []
    
    for i in range(length):
        argument = " " + str(i) + " "
        suffix = "_pthread"
        tempo1 = tm.clock_gettime(tm.CLOCK_MONOTONIC_RAW)
        os.system(directory + test_list[arg_test] + suffix + argument)
        tempo2 = tm.clock_gettime(tm.CLOCK_MONOTONIC_RAW)
        tempo = tempo2 - tempo1
        res += [tempo]
    final_res = []
    for i in range(0,1000,20):
        value = 0
        for j in range(20):
            value += res[i + j]
        final_res += [value]
    plt.plot(final_res)
    plt.show()

print_graph_pthread()
        
