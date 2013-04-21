Synergy30+ Evaluation Release  (c)Temple University 1995-2013
=============================================================

Synergy 3.0+ is a prototype of SM-HPC (Statistic Multiplexed High Performance Computing) System. Unlike existing HPC systems, SM-HPC focuses on harnessing volatile resources. The goal is to allow SM-HPC applications to gain performance and reliability at the same time when adding computing nodes and networks.

The Synergy3.0 runtime and source code are installed under the directory "synergy".

The sample applications are packaged in a single apps.tar.gz file. Uncompressed, there are three subdirectories here:

1) selftests -- Passive Object Test Programs (removed from this directory)
	ptest 		-- a C pipe object test program.
	ftest 		-- a C file object test program.
	ttest 		-- a C tuple space object test program.
	deptest 	-- an integrated parallel C test system for
				all types of objects.
	fortran		-- Fortran and C/Fortran mixed tests.

2. ssc -- Solution space compact rograms (no superlinear speedup potentials)
	fractal 	-- a parallel Mandelbrot system,
				an ideal testbed for load balancing algorithms.
	xepp		-- a parallel interactive Mandelbrot system,
				a program transplanted from PVM.
	matrix		-- a parallel matrix multiplication system. (Tested in 2013)
	gauss		-- a parallel linear solver using Gaussian Elimination,
				a demo of complex program-to-program 
				synchronization and communication using 
				multiple tuple space objects.
	nqueen		-- a parallel N-Queen solver,
				a deterministic recursive program example.
	laplace		-- a parallel heat transfer simulation program using
				a relaxed fixed point algorithm.
	raytrace	-- a parallel ray tracer using POV and Synergy.
	albm		-- a matrix multiplication example. This program
				can automatically balance the fluctuating
        work loads of heterogeneous processors
  			delivering near optimal speedup.
3. nssc -- Non-solution space compact programs (these applications can generate superlinear speedups)
  msort 		-- a parallel C mergesort system, a demonstration of pipe usage.
	knapsack	-- a parallel 0/1 knapsack decision solver (NP-complete). 
	subsetsum	-- a parallel sum of subsets solver (NP-complete).
	tsppar          -- a parallel Traveling Salesman decision solver (NP-complete).

Note:
  These examples are projects from CIS undergraduate and graduate students' projects. They are only rudimentally tested. The idea is to show how to compose data parallel applications that can leverage the Statistic Multiplexed Computing architecture. Please report any problems and comments to shi@temple.edu.
				
