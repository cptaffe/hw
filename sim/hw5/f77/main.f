
*
* Homework 5
* CPSC 4399-11; Special Topics: Monte Carlo Simulation
* Professor Minsker
*
* Connor P. Taffe
* T no. 00563742
*

      program main
          print *, 'Simulated: ', sim()
      end

      real function distribution(days)
          integer days, rnd, dist(days)
* Zero out dist array
          do 10 i=1, days
              dist(i) = 0
10            continue

* Keep tally of unique days
          do 20 i=1, days
              rnd = int(rand() * (days-1))+1
              dist(rnd) = dist(rnd)+1
              if (dist(rnd) == 1) then
                  distribution = distribution+1
              else if (dist(rnd) > 1) then
                  distribution = distribution-1
              end if
20            continue
              distribution = 1-distribution/days
              print *, distribution
          return
      end

* Simulation function,
* returns mean
      real function sim()
          integer days, loops
          parameter(days = 365, loops = 1000)

          do 10 i=1, days
* Loop through simulations
              do 20 j=1, loops
* Sum unique days for each 
                  sim = sim + distribution(days)
20                continue
10            continue
          sim = sim/loops
          return
      end
