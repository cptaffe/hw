
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

* Returns number of days that were unique out of days.
      integer function distribution(days)
          integer days, rnd
          integer dist(days)

          distribution = 0
* Zero out dist array.
          do 10 i=1, days
              dist(i) = 0
10            continue
* Keep tally of unique days.
          do 20 i=1, days
* Random number on range 1-365.
              rnd = int(rand() * (days-1))+1
              if (dist(rnd) == 0) then
* Increment one on unseen they are unique for now.
                  distribution = distribution+1
              else if (dist(rnd) == 1) then
* Decrement one on previously seen they are no long unique.
                  distribution = distribution-1
              end if
              dist(rnd) = dist(rnd)+1
20            continue
          return
      end

* Simulation function, returns mean.
      double precision function sim()
          integer days, loops, distribution
          parameter(days = 365, loops = 100000)
          sim = 0
* Loop through simulations.
          do 10 j=1, loops
* Sum unique days for distribution.
              sim = sim + distribution(days)
10            continue
          sim = days-sim/loops
          return
      end
