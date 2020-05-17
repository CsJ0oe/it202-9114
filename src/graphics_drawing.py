import matplotlib.pyplot as plt
import time as tm
import os
import numpy as np
import copy
import sys

test_list = ["01-main", "02-switch", "11-join", "12-join-main", "21-create-many", "22-create-many-recursive", "23-create-many-once", "31-switch-many", "32-switch-many-join", "51-fibonacci"]
arg_list = [(4,1), (5,1), (6,1), (7,2), (8,2), (9,1)] #tests which need n arguments to be given !!! THIS LIST IS SORTED FOR PERFORMANCES !!!
arg = sys.argv
nb_test = 10
arg_test = int(arg[1])
nb_arg = 0
if (arg_test >= 4):
    nb_arg = arg_list[arg_test-4][1]
length = int(arg[2])
directory_thread = "../install/bin/"
directory_pthread = "../obj/"
directory= "./install/bin/"


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
                argument += "10"  + " "
            arg_pointer += 1
            
                
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC)
        os.system(directory_thread + test_list[i] + argument)
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC) - tempo
        res += [tempo]
        if tempo > maxi:
            maxi = tempo
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC)
        os.system(directory_pthread + test_list[i] + argument)
        tempo = tm.clock_gettime(tm.CLOCK_MONOTONIC) - tempo
        res_pthread += [tempo]
        if tempo > maxi:
            maxi = tempo
    width_ = 0.6
    plt.bar(range(length), res, width = width_, color = 'yellow', edgecolor = 'blue', linewidth = 2)
    plt.bar([x + width_ for x in range(length)], res_pthread, width = width_, color = 'red', edgecolor = 'blue', linewidth = 2)
    plt.show()
    
def print_graph_thread():
    res_thread = []
    res_pthread = []
    suffix = "_pthread"
    for i in range(length):
        value_thread = 0
        value_pthread = 0
        for j in range(nb_test):
            argument = " "
            for k in range(nb_arg):
                argument += str(i) + " "
            tempo1 = tm.clock_gettime(tm.CLOCK_MONOTONIC_RAW)
            os.system(directory_thread + test_list[arg_test] + argument)
            tempo2 = tm.clock_gettime(tm.CLOCK_MONOTONIC_RAW)
            tempo = tempo2 - tempo1
            value_thread += tempo
            tempo1 = tm.clock_gettime(tm.CLOCK_MONOTONIC_RAW)
            os.system(directory_pthread + test_list[arg_test] + suffix + argument)
            tempo2 = tm.clock_gettime(tm.CLOCK_MONOTONIC_RAW)
            tempo = tempo2 - tempo1
            value_pthread += tempo
        value_thread = value_thread / nb_test
        value_pthread = value_pthread / nb_test
        res_thread += [value_thread]
        res_pthread += [value_pthread]
    if (arg_test == 9):
        plt.plot(res_thread, color = 'blue', label='thread')
        plt.plot(res_pthread, color='purple', label='pthread')
    else:
        final_res_thread = copy.deepcopy(res_thread)
        for i in range(20,len(res_thread)-20):
            value = 0
            for j in range(40):
                value += res_thread[i-20+j]
            final_res_thread[i] = value/40
        final_res_pthread = copy.deepcopy(res_pthread)
        for i in range(20,len(res_pthread)-20):
            value = 0
            for j in range(40):
                value += res_pthread[i-20+j]
            final_res_pthread[i] = value/40
        plt.plot(final_res_thread, color = 'blue', label='thread')
        plt.plot(final_res_pthread, color='purple', label='pthread')
    plt.legend()
    plt.show()

print_graph_thread()
        
