####OPERATING SYSTEMS- PA3

#####GROUP MEMBERS:
Navya Ganta \
Sai Pratyusha Attanti

####Instructions to compile and run the program
Instructions to compile and run the project:
1. To run individual tests:\
Run `make` to compile the code\
To Run:
```./virtmem <npages> <nframes> <rand|fifo|custom> <sort|scan|focus>```
2. To Generate the data for the Graphs:\
Run `make` to compile the code
To Run:
```./virtmem <rand|fifo|custom> <sort|scan|focus>```\
The above command will run the tests for fixed pages(100) and varying frames(1-100) and generate the page faults and disk writes for each combination and store them in text files.
3. To generate graphs from the above data:\
Run: ```python3 graph.py```

The comparison and the analysis of the above results can be found in report.pdf file