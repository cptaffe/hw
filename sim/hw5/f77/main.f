
* This program is an implementation of
* Monte Carlo Simulation's Homework 5.
      program main
          print *, 'Simulated: ', sim()
      end

* Function to do simulation
      real function sim()
          integer days, loops
          parameter(days = 365, loops = 1000)
          integer s, dist(days), rnd

          s = 0
          do 10 i=1, days
* Do loops simulations
              do 30 j=1, loops
* Zero dist array
                  do 40 k=1, days
                      dist(k) = 0
40                    continue
* Randomly generate birthdays
                  do 50 k=1, days
* Random number between 0 and 1, multiplied by n-1,
* and shifted up by 1 to be between 1 and n inclusive.
                      rnd = int(rand() * (days-1))+1
                      dist(rnd) = dist(rnd)+1
* Keep updated sum based on dist
                      if (dist(rnd) .eq. 1) then
                          s = s + 1
                      else if (dist(rnd) .gt. 1) then
                          s = s - 1
                      end if
50                    continue
30                continue
* Calculate probability
10            continue
          print *,s
          sim = days-real(s)/(loops*days)
          return
      end

