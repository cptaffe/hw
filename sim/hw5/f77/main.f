
* This program is an implementation of
* Monte Carlo Simulation's Homework 5.
      program main
          print *, 'Simulated: ', sim()
      end

* Function to do simulation
      real function sim()
          integer days, loops
          parameter(days = 365, loops = 1000)
          integer dist(days), rnd
          real pr, probs(loops)

          do 10 i=1, days
* Zero probs array
              do 20 j=1, loops
                  probs(j) = 0.0
20                continue
* Do loops simulations
              do 30 j=1, loops
* Zero dist array
                  do 40 k=1, days
                      dist(k) = 0
40                    continue
* Randomly generate
                  do 50 k=1, days
* Random number between 0 and 1, multiplied by n-1,
* and shifted up by 1 to be between 1 and n inclusive.
                      rnd = int(rand() * real(days-1))+1
                      dist(rnd) = dist(rnd) + 1
50                    continue
* Count distinct answers
                  do 60 k=1, days
                      if (dist(k) .eq. 1) then
                          probs(k) = probs(k) + 1
                      end if
60                    continue
30                continue
* Calculate probability
              pr = 0.0
              do 14 k=1, loops
                  pr = pr + probs(k)
14                continue
              sim = sim + (1.0-(pr/(real(loops)*real(days))))
10            continue
          return
      end
