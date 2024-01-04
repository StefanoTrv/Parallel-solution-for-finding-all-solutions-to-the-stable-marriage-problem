CC = gcc
NVCC = nvcc
#CUDAFLAGS = set your hw flags here


#To test serial version with single instances from a .txt file. Examples can be found in \tests\.	
serial.o: serial_solutions_finder.c
	$(CC) serial_solutions_finder.c -o serial.o

serial: serial.o
	.\serial $(input_file) $(output_file)


#To test parallel version with single instances from a .txt file. Examples can be found in \tests\.	
parallel.o: parallel_solutions_finder.cu
	$(NVCC) $(CUDAFLAGS) parallel_solutions_finder.cu -o parallel
	
parallel: parallel.o
	.\parallel $(input_file) $(output_file)


#To test both versions
solutions_comparator.o: solutions_comparator.cu
	$(NVCC) $(CUDAFLAGS) solutions_comparator.cu -o solutions_comparator

comparator: solutions_comparator.o
	.\solutions_comparator $(instances) $(iterations)


clean:
	del $(wildcard *.exe) $(wildcard *.exp) $(wildcard *.pdb) $(wildcard *.lib)