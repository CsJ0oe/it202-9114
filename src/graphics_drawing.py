import matplotlib.pyplot as plt
import time as tm
import os
import numpy as np


test_list = ["01-main", "02-switch", "11-join", "12-join-main", "21-create-many", "22-create-many-recursive", "23-create-many-once", "31-switch-many", "32-switch-many-join", "51-fibonacci"]
arg_list = [(4,1), (5,1), (6,1), (7,2), (8,2), (9,2)] #tests which need n arguments to be given !!! THIS LIST IS SORTED FOR PERFORMANCES !!!
arg = "10" #TODO: improve the choice of argument (depending on the test)
directory = "./install/bin/"

def print_graph():
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
                argument += arg + " "
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





print_graph()
        
