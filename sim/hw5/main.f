
c This program is an implementation of Monte Carlo Simulation's Homework 5.
      program main
          write(*,*) 365, " Simulated: ", sim()
      end

c Function to do simulation
      real function sim()
          integer days, loops
          parameter(days = 365, loops = 1000)
          integer dist(days), rnd
          real pr, probs(loops), rnd2

          do 20 i=1, days

c Zero probs array
              do 19 j=1, loops
                  probs(j) = 0.0
19                continue

c Do loops simulations
              do 18 j=1, loops

c Zero dist array
                  do 17 k=1, loops
                      dist(k) = 0
17                    continue

c Randomly generate
                  do 16 k=1, days
c GNU Fortran extension rand(1) returns a random float between 0 and 1.
                      rnd2 = int(rand(1) * real(days))
                      write(*,*) "random: ", rnd
                      dist(rnd) = dist(rnd) + 1
16                    continue

c Count distinct answers
                  do 15 k=1, days
                      if (dist(k) .eq. 1) then
                          probs(k) = probs(k) + 1
                      end if
15                    continue

18          continue

c Calculate probability
              pr = 0.0
              do 14 k=1, loops
                  pr = pr + probs(k)
14                continue

              sim = sim + (1-(pr/(real(loops)*real(days))))

20            continue
          return
      end
