Synergy30+ Evaluation Release  (c)Temple University 1995-2013
=============================================================

Synergy 3.0+ is a prototype of SM-HPC (Statistic Multiplexed High Performance Computing) System. Unlike existing HPC systems, SM-HPC focuses on harnessing volatile resources. The goal is to allow SM-HPC applications to gain performance and reliability at the same time when adding computing nodes and networks.

The Synergy3.0 runtime is under ~/synergy/bin. It should be included in $PATH. $SNG_PATH should point to ~/synergy.  Source code are installed under the directory "synergy". A single "makefile" compiles all. File "GettingStarted" is a user's manual primer.

The sample applications are packaged in a separate Synergy3.0+Apps.tar.gz file. Uncompressed, there are two subdirectories:

ssc -- Solution space compact programs (no superlinear speedup potentials): fractal 	-- a parallel Mandelbrot application with X-Windows,	an ideal massive embarrisingly parallel application. xepp		-- a parallel interactive Mandelbrot application, a program transplanted from PVM. matrix		-- a parallel matrix multiplication system. (Tested in 2013).
gauss		-- a parallel linear solver using Gaussian Elimination, a demo of complex program-to-program 
synchronization and communication using	multiple tuple space objects. nqueen		-- a parallel N-Queen solver,
a deterministic recursive program example. laplace		-- a parallel heat transfer simulation program using
a relaxed fixed point algorithm. raytrace	-- a parallel ray tracer using POV and Synergy.  albm		-- a matrix multiplication example. This program can automatically balance the fluctuating work loads of heterogeneous processors delivering near optimal speedup.

nssc -- Non-solution space compact programs (these applications can generate superlinear speedups). knapsack	-- a parallel 0/1 knapsack decision solver (NP-complete). 	subsetsum	-- a parallel sum of subsets solver (NP-complete).
tsppar -- a parallel Traveling Salesman decision solver (NP-complete).

Note:
These examples are only rudimentally tested. The idea is to show how to compose statistic multiplexed parallel applications that can leverage the volatile computing resources. Please report any problems and comments to shi@temple.edu.
				
