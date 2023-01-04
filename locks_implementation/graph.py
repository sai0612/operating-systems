from matplotlib import pyplot as plt
critical_section_1 = []
critical_section_2 = []
spin_lock_times = []
lock_times = []
 
f = open('spin_lock_file.txt','r')
for row in f:
    row = row.split(' ')
    critical_section_1.append(row[0])
    spin_lock_times.append(int(row[1]))
f.close()
   
f = open('lock_file.txt','r')
for row in f:
    row = row.split(' ')
    critical_section_2.append(row[0])
    lock_times.append(int(row[1]))
f.close()
 
fig, ax = plt.subplots()

plt.title('Spin Locks VS Normal locks')
plt.xlabel('Critical section load')
plt.ylabel('Time Taken(ms)')
ax.plot(critical_section_1, spin_lock_times, label = "Spin Lock")
ax.plot(critical_section_1, lock_times, label = "Mutex Lock")
plt.xticks(rotation=90)
plt.legend()
plt.show()
