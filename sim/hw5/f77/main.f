
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

* Returns number of days that were unique out of days.
      integer function distribution(days)
          integer days, rnd
          logical dist(days)
          
          distribution = 0
* Zero out dist array
          do 10 i=1, days
              dist(i) = .false.
10            continue
* Keep tally of unique days
          do 20 i=1, days
              rnd = int(rand() * (days-1))+1
              if (.not. dist(rnd)) then
* Increment one on unseen
* they are unique for now
                  distribution = distribution+1
                  dist(rnd) = .true.
              else
* Decrement one on previously seen
* they are no longer unique
                  distribution = distribution-1
              end if
20            continue
          return
      end

* Simulation function,
* returns mean
      real function sim()
          integer days, loops, distribution
          parameter(days = 365, loops = 10000)
          sim = 0
* Loop through simulations
          do 10 j=1, loops
* Sum unique days for each
              sim = sim + distribution(days)
10            continue
          sim = days-sim/loops
          return
      end
