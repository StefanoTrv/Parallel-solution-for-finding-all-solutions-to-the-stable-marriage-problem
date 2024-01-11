CC = gcc
NVCC = nvcc
#CUDAFLAGS = set your hw flags here
SEED = $(seed)

DEF_ITERATIONS = 51


#To test serial version with single instances from a .txt file. Examples can be found in \tests\.	
serial.o: serial_solutions_finder.c
	$(CC) serial_solutions_finder.c -o serial

serial: serial.o
	.\serial $(input_file) $(output_file)


#To launch multiple pseudo-random tests genarated on serial version, by giving instance dimension and number of iterations. 
#Also, by specifying mode, time taken by the algorithm can be measured. Otherwise, mode should be "classic".
tester.o: tester.c
	$(CC) tester.c -o tester

tester: tester.o
ifeq ($(seed),default)
#	@echo $(SEED)
	.\tester $(instance) $(iterations) $(mode) $(output_file)
else
#	@echo $(SEED)
	.\tester $(instance) $(iterations) $(mode) $(output_file) $(SEED)
endif


#To launch multiple pseudo-random tests genarated on serial version, starting from instance=500, step=500. Useful for time analysis.
serial_analysis: tester.o
	for %%i in (500 1000 1500 2000 2500 3000 3500 4000 4500 5000) do .\tester %%i $(DEF_ITERATIONS) times output\serial_tests\%%i.txt
	

#To test parallel version with single instances from a .txt file. Examples can be found in \tests\.	
parallel.o: parallel_solutions_finder.cu
	$(NVCC) $(CUDAFLAGS) parallel_solutions_finder.cu -o parallel
	
parallel: parallel.o
	.\parallel $(input_file) $(output_file)


#To launch multiple pseudo-random tests genarated on parallel version, by giving instance dimension and number of iterations. 
#Also, by specifying mode, time taken by the algorithm can be measured. Otherwise, mode should be "classic".
CUDA_tester.o: CUDA_tester.cu
	$(NVCC) $(CUDAFLAGS) CUDA_tester.cu -o CUDAtester

cuda_tester: CUDA_tester.o
ifeq ($(seed),default)
#	@echo $(SEED)
	.\CUDAtester $(instance) $(iterations) $(mode) $(output_file)
else
#	@echo $(SEED)
	.\CUDAtester $(instance) $(iterations) $(mode) $(output_file) $(SEED)
endif


#To launch multiple pseudo-random tests genarated on parallel version, starting from instance=500, step=500. Useful for time analysis.
parallel_analysis: CUDA_tester.o
	for %%i in (500 1000 1500 2000 2500 3000 3500 4000 4500 5000) do .\CUDAtester %%i $(DEF_ITERATIONS) times output\parallel_tests\%%i.txt


#To test both versions
solutions_comparator.o: solutions_comparator.cu
	$(NVCC) $(CUDAFLAGS) solutions_comparator.cu -o solutions_comparator


#To compare both versions
comparator: solutions_comparator.o
ifeq ($(seed),default)
#	@echo $(SEED)
	.\solutions_comparator $(instance) $(iterations)
else
#	@echo $(SEED)
	.\solutions_comparator $(instance) $(iterations) $(SEED)
endif


#A demo. For $(DEF_ITERATIONS) times, it launches %%i pseudo-random generated instance on solutions_comparator, which means first calling
#serial and then parallel version. After that, both results are compared each other. If any errors are found, they will be printed on prompt.
demo: solutions_comparator.o
	for %%i in (500 1000 1500 2000 2500 3000 3500 4000) do .\solutions_comparator %%i 50 default


clean:
	del $(wildcard *.exe) $(wildcard *.exp) $(wildcard *.pdb) $(wildcard *.lib) $(wildcard *.o)