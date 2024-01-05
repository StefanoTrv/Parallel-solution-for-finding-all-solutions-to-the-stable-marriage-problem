CC = gcc
NVCC = nvcc
#CUDAFLAGS = set your hw flags here


#To test serial version with single instances from a .txt file. Examples can be found in \tests\.	
serial.o: serial_solutions_finder.c
	$(CC) serial_solutions_finder.c -o serial

serial: serial.o
	.\serial $(input_file) $(output_file)


#To launch multiple pseudo-random tests genareted on serial version, by giving instance dimension and number of iterations. 
#Also, by specifying mode, time taken by the algorithm can be measured. Otherwise, mode should be "classic".
tester.o: tester.c
	$(CC) tester.c -o tester

tester: tester.o
	.\tester $(instances) $(iterations) $(mode) $(output_file)


#To test parallel version with single instances from a .txt file. Examples can be found in \tests\.	
parallel.o: parallel_solutions_finder.cu
	$(NVCC) $(CUDAFLAGS) parallel_solutions_finder.cu -o parallel
	
parallel: parallel.o
	.\parallel $(input_file) $(output_file)


#To launch multiple pseudo-random tests genareted on parallel version, by giving instance dimension and number of iterations. 
#Also, by specifying mode, time taken by the algorithm can be measured. Otherwise, mode should be "classic".
CUDA_tester.o: CUDA_tester.cu
	$(NVCC) $(CUDAFLAGS) CUDA_tester.cu -o CUDAtester

cuda_tester: CUDA_tester.o
	.\CUDAtester $(instances) $(iterations) $(mode) $(output_file)


#To test both versions
solutions_comparator.o: solutions_comparator.cu
	$(NVCC) $(CUDAFLAGS) solutions_comparator.cu -o solutions_comparator

comparator: solutions_comparator.o
	.\solutions_comparator $(instances) $(iterations)


clean:
	del $(wildcard *.exe) $(wildcard *.exp) $(wildcard *.pdb) $(wildcard *.lib) $(wildcard *.o)