
*
* Homework 5
* CPSC 4399-11; Special Topics: Monte Carlo Simulation
* Professor Minsker
* Connor P. Taffe
* T no. 00563742
*

      program main
          double precision sim
          print *, 'Simulated: ', sim()
      end

*
* Generates n random numbers on the range [1,n].
* returns the cardinality of the set of unique
* numbers generated.
*
      integer function unique(n)
          integer n, rnd
          integer seen(n)
          external rand
          double precision rand

* Intialize local variables.
          unique = 0
          do 10 i=1, n
              seen(i) = 0
10            continue


*
* Loop n times, generating n random numbers
* on the range [1, n]. Returns the number of unique
* numbers (numbers only generated once).
*
          do 20 i=1, n
*
* Random number on range [1, 365].
* The rand function is implemented in C code using the C
* standard library's rand function, which returns
* on the range [0, RAND_MAX].
* The rand function divides that by RAND_MAX cast to a double
* to get a double precision floating point on the
* range [0, 1].
* ([0, 1] * (n-1))+1 => [1,n].
*
              rnd = int(rand() * (n-1))+1
*
* "dist" is an array of size n, indexed on
* the range [1, n]. For each index, the corresponding
* entry is the number of times that index has been generated
* by this loop.
* On no previous generations, it is a unique value,
* so the unique count is incremented.
* On one previous generation, it is no longer unique,
* so the unique count is decremented.
*
              if (seen(rnd) == 0) then
                  unique = unique+1
              else if (seen(rnd) == 1) then
                  unique = unique-1
              end if
              seen(rnd) = seen(rnd)+1

20            continue
          return
      end

* Simulation function, returns mean.
      double precision function sim()
          integer n, m, unique
          parameter(m = 365, n = 100000)
          sim = 0
*
* Loop over n simulations.
* Sums unique values out of m trials.
*
          do 10 j=1, n
              sim = sim + unique(m)
10            continue
*
* Value of the simulation is the number of trails
* minus the total uniques out of the total simulations.
*
          sim = m-sim/n
          return
      end
