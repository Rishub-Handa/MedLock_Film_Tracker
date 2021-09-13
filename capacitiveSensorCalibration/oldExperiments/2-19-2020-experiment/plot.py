import matplotlib.pyplot as plt
import os
import csv




print(os.walk('.'))

figureCount = 0


for root, dirs, files in os.walk(".", topdown=False):

    for name in files:

        figureCount += 1

      #print(os.path.join(root, name))
      #print(name)
      #print(os.path.join(root, name))
        time = []
        raw = [] 
        averaging= []
        derivative = []


        #plt.figure(figureCount)

        with open(root + '/OSM.csv','r') as csvfile:
            plots = csv.reader(csvfile, delimiter=',')
            for row in plots:
                time.append(int(row[0]))
                raw.append(int(row[1]))
                averaging.append(int(row[2]))
                derivative.append(int(row[3]))

        
        plt.plot(time,averaging)
        plt.ylabel('ADC Voltage')
        plt.title = (name + '/OSM.csv')
        plt.show()

        time = []
        raw = [] 
        averaging= []
        derivative = []

        #plt.plot(time, raw)
        #plt.subplot(time, averaging)
        #plt.subplot(time, derivative)        

        with open(root + '/SIM.csv','r') as csvfile:
            plots = csv.reader(csvfile, delimiter=',')
            for row in plots:
                time.append(int(row[0]))
                raw.append(int(row[1]))
                averaging.append(int(row[2]))
                derivative.append(int(row[3]))


        time = []
        raw = [] 
        averaging= []
        derivative = []


        with open(root + '/SIM.csv','r') as csvfile:
            plots = csv.reader(csvfile, delimiter=',')
            for row in plots:
                time.append(int(row[0]))
                raw.append(int(row[1]))
                averaging.append(int(row[2]))
                derivative.append(int(row[3]))

        time = []
        raw = [] 
        averaging= []
        derivative = []


'''



time = []
averaging = []
derivative = []

with open( root + '/OSM.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        time.append(int(row[0]))
        averaging.append(int(row[1]))

plt.plot([1, 2, 3, 4])
plt.ylabel('some numbers')
plt.show()
'''