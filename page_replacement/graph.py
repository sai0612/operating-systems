from matplotlib import pyplot as plt
import os

if os.path.exists("fifo_sort.txt"):
	frames = []
	page_faults = []
	disk_writes = []
	  
	f = open('fifo_sort.txt','r')
	for row in f:
    		#print(row)
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		page_faults.append(int(row[1]))
    		disk_writes.append(int(row[3]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Algorithm: FIFO, Program: SORT, (Frames VS Page_faults) && (Frames VS disk_writes)')
	plt.xlabel('Frames')
	ax.scatter(frames, page_faults, s=5, label = "Page Faults")
	ax.scatter(frames, disk_writes, s=5, label = "Disk Writes")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()
	
if os.path.exists("fifo_scan.txt"):
	frames = []
	page_faults = []
	disk_writes = []
  
	f = open('fifo_scan.txt','r')
	for row in f:
    		#print(row)
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		page_faults.append(int(row[1]))
    		disk_writes.append(int(row[3]))
	f.close()

	fig, ax = plt.subplots()

	plt.title('Algorithm: FIFO, Program: SCAN, (Frames VS Page_faults) && (Frames VS disk_writes)')
	plt.xlabel('Frames')
	ax.scatter(frames, page_faults, s=5, label = "Page Faults")
	ax.scatter(frames, disk_writes, s=5, label = "Disk Writes")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()
	
if os.path.exists("fifo_focus.txt"):
	frames = []
	page_faults = []
	disk_writes = []
  
	f = open('fifo_focus.txt','r')
	for row in f:
    		#print(row)
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		page_faults.append(int(row[1]))
    		disk_writes.append(int(row[3]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Algorithm: FIFO, Program: FOCUS, (Frames VS Page_faults) && (Frames VS disk_writes)')
	plt.xlabel('Frames')
	ax.scatter(frames, page_faults, s=5, label = "Page Faults")
	ax.scatter(frames, disk_writes, s=5, label = "Disk Writes")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()

if os.path.exists("rand_sort.txt"):
	frames = []
	page_faults = []
	disk_writes = []
	  
	f = open('rand_sort.txt','r')
	for row in f:
    		#print(row)
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		page_faults.append(int(row[1]))
    		disk_writes.append(int(row[3]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Algorithm: RAND, Program: SORT, (Frames VS Page_faults) && (Frames VS disk_writes)')
	plt.xlabel('Frames')
	ax.scatter(frames, page_faults, s=5, label = "Page Faults")
	ax.scatter(frames, disk_writes, s=5, label = "Disk Writes")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()
	
if os.path.exists("rand_scan.txt"):
	frames = []
	page_faults = []
	disk_writes = []
	  
	f = open('rand_scan.txt','r')
	for row in f:
    		#print(row)
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		page_faults.append(int(row[1]))
    		disk_writes.append(int(row[3]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Algorithm: RAND, Program: SCAN, (Frames VS Page_faults) && (Frames VS disk_writes)')
	plt.xlabel('Frames')
	ax.scatter(frames, page_faults, s=5, label = "Page Faults")
	ax.scatter(frames, disk_writes, s=5, label = "Disk Writes")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()
	
if os.path.exists("rand_focus.txt"):
	frames = []
	page_faults = []
	disk_writes = []
  
	f = open('rand_focus.txt','r')
	for row in f:
    		#print(row)
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		page_faults.append(int(row[1]))
    		disk_writes.append(int(row[3]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Algorithm: RAND, Program: FOCUS, (Frames VS Page_faults) && (Frames VS disk_writes)')
	plt.xlabel('Frames')
	ax.scatter(frames, page_faults, s=5, label = "Page Faults")
	ax.scatter(frames, disk_writes, s=5, label = "Disk Writes")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()
	
if os.path.exists("custom_sort.txt"):
	frames = []
	page_faults = []
	disk_writes = []
  
	f = open('custom_sort.txt','r')
	for row in f:
    		#print(row)
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		page_faults.append(int(row[1]))
    		disk_writes.append(int(row[3]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Algorithm: CUSTOM, Program: SORT, (Frames VS Page_faults) && (Frames VS disk_writes)')
	plt.xlabel('Frames')
	ax.scatter(frames, page_faults, s=5, label = "Page Faults")
	ax.scatter(frames, disk_writes, s=5, label = "Disk Writes")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()

if os.path.exists("custom_scan.txt"):
	frames = []
	page_faults = []
	disk_writes = []
	  
	f = open('custom_scan.txt','r')
	for row in f:
    		#print(row)
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		page_faults.append(int(row[1]))
    		disk_writes.append(int(row[3]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Algorithm: CUSTOM, Program: SCAN, (Frames VS Page_faults) && (Frames VS disk_writes)')
	plt.xlabel('Frames')
	ax.scatter(frames, page_faults, s=5, label = "Page Faults")
	ax.scatter(frames, disk_writes, s=5, label = "Disk Writes")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()

if os.path.exists("custom_focus.txt"):
	frames = []
	page_faults = []
	disk_writes = []
  
	f = open('custom_focus.txt','r')
	for row in f:
    		#print(row)
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		page_faults.append(int(row[1]))
    		disk_writes.append(int(row[3]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Algorithm: CUSTOM, Program: FOCUS, (Frames VS Page_faults) && (Frames VS disk_writes)')
	plt.xlabel('Frames')
	ax.scatter(frames, page_faults, s=5, label = "Page Faults")
	ax.scatter(frames, disk_writes, s=5, label = "Disk Writes")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()
	
if os.path.exists("fifo_sort.txt") and os.path.exists("rand_sort.txt") and os.path.exists("custom_sort.txt"):
	frames = []
	fifo_page_faults = []
	rand_page_faults = []
	custom_page_faults = []
  
	f = open('fifo_sort.txt','r')
	for row in f:
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		fifo_page_faults.append(int(row[1]))
	f.close()

	f = open('rand_sort.txt','r')
	for row in f:
    		row = row.split(' ')
    		rand_page_faults.append(int(row[1]))
	f.close()

	f = open('custom_sort.txt','r')
	for row in f:
    		row = row.split(' ')
    		custom_page_faults.append(int(row[1]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Program: SORT, FRAMES VS PAGE_FAULTS')
	plt.xlabel('Frames')
	plt.ylabel('Page Faults')
	ax.plot(frames, fifo_page_faults, label = "FIFO")
	ax.plot(frames, rand_page_faults, label = "RANDOM")
	ax.plot(frames, custom_page_faults, label = "CUSTOM")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()

if os.path.exists("fifo_scan.txt") and os.path.exists("rand_scan.txt") and os.path.exists("custom_scan.txt"):
	frames = []
	fifo_page_faults = []
	rand_page_faults = []
	custom_page_faults = []
  
	f = open('fifo_scan.txt','r')
	for row in f:
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		fifo_page_faults.append(int(row[1]))
	f.close()

	f = open('rand_scan.txt','r')
	for row in f:
    		row = row.split(' ')
    		rand_page_faults.append(int(row[1]))
	f.close()

	f = open('custom_scan.txt','r')
	for row in f:
    		row = row.split(' ')
    		custom_page_faults.append(int(row[1]))
	f.close()

  
	fig, ax = plt.subplots()


	plt.title('Program: SCAN, FRAMES VS PAGE_FAULTS')
	plt.xlabel('Frames')
	plt.ylabel('Page Faults')
	ax.plot(frames, fifo_page_faults, label = "FIFO")
	ax.plot(frames, rand_page_faults, label = "RANDOM")
	ax.plot(frames, custom_page_faults, label = "CUSTOM")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()

if os.path.exists("fifo_focus.txt") and os.path.exists("rand_focus.txt") and os.path.exists("custom_focus.txt"):	
	frames = []
	fifo_page_faults = []
	rand_page_faults = []
	custom_page_faults = []
  
	f = open('fifo_focus.txt','r')
	for row in f:
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		fifo_page_faults.append(int(row[1]))
	f.close()

	f = open('rand_focus.txt','r')
	for row in f:
    		row = row.split(' ')
    		rand_page_faults.append(int(row[1]))
	f.close()

	f = open('custom_focus.txt','r')
	for row in f:
    		row = row.split(' ')
    		custom_page_faults.append(int(row[1]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Program: FOCUS, FRAMES VS PAGE_FAULTS')
	plt.xlabel('Frames')
	plt.ylabel('Page Faults')
	ax.plot(frames, fifo_page_faults, label = "FIFO")
	ax.plot(frames, rand_page_faults, label = "RANDOM")
	ax.plot(frames, custom_page_faults, label = "CUSTOM")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()
	
if os.path.exists("fifo_sort.txt") and os.path.exists("rand_sort.txt") and os.path.exists("custom_sort.txt"):	
	frames = []
	fifo_disk_writes = []
	rand_disk_writes = []
	custom_disk_writes = []
  
	f = open('fifo_sort.txt','r')
	for row in f:
    		row = row.split(' ')
    		frames.append(int(row[0]))
    		fifo_disk_writes.append(int(row[3]))
	f.close()

	f = open('rand_sort.txt','r')
	for row in f:
    		row = row.split(' ')
    		rand_disk_writes.append(int(row[3]))
	f.close()

	f = open('custom_sort.txt','r')
	for row in f:
    		row = row.split(' ')
    		custom_disk_writes.append(int(row[3]))
	f.close()

  
	fig, ax = plt.subplots()

	plt.title('Program: SORT, FRAMES VS DISK_WRITES')
	plt.xlabel('Frames')
	plt.ylabel('Disk Writes')
	ax.plot(frames, fifo_disk_writes, label = "FIFO")
	ax.plot(frames, rand_disk_writes, label = "RANDOM")
	ax.plot(frames, custom_disk_writes, label = "CUSTOM")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()
	
if os.path.exists("fifo_scan.txt") and os.path.exists("rand_scan.txt") and os.path.exists("custom_scan.txt"):
	frames = []
	fifo_disk_writes = []
	rand_disk_writes = []
	custom_disk_writes = []
	  
	f = open('fifo_scan.txt','r')
	for row in f:
	    row = row.split(' ')
	    frames.append(int(row[0]))
	    fifo_disk_writes.append(int(row[3]))
	f.close()

	f = open('rand_scan.txt','r')
	for row in f:
	    row = row.split(' ')
	    rand_disk_writes.append(int(row[3]))
	f.close()

	f = open('custom_scan.txt','r')
	for row in f:
	    row = row.split(' ')
	    custom_disk_writes.append(int(row[3]))
	f.close()

	  
	fig, ax = plt.subplots()

	plt.title('Program: SCAN, FRAMES VS DISK_WRITES')
	plt.xlabel('Frames')
	plt.ylabel('Disk Writes')
	ax.plot(frames, fifo_disk_writes, label = "FIFO")
	ax.plot(frames, rand_disk_writes, label = "RANDOM")
	ax.plot(frames, custom_disk_writes, label = "CUSTOM")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()

if os.path.exists("fifo_focus.txt") and os.path.exists("rand_focus.txt") and os.path.exists("custom_focus.txt"):
	frames = []
	fifo_disk_writes = []
	rand_disk_writes = []
	custom_disk_writes = []
	  
	f = open('fifo_focus.txt','r')
	for row in f:
	    row = row.split(' ')
	    frames.append(int(row[0]))
	    fifo_disk_writes.append(int(row[3]))
	f.close()

	f = open('rand_focus.txt','r')
	for row in f:
	    row = row.split(' ')
	    rand_disk_writes.append(int(row[3]))
	f.close()

	f = open('custom_focus.txt','r')
	for row in f:
	    row = row.split(' ')
	    custom_disk_writes.append(int(row[3]))
	f.close()

	  
	fig, ax = plt.subplots()

	plt.title('Program: FOCUS, FRAMES VS DISK_WRITES')
	plt.xlabel('Frames')
	plt.ylabel('Disk Writes')
	ax.plot(frames, fifo_disk_writes, label = "FIFO")
	ax.plot(frames, rand_disk_writes, label = "RANDOM")
	ax.plot(frames, custom_disk_writes, label = "CUSTOM")
	plt.xticks(rotation=90)
	plt.legend()
	plt.show()

