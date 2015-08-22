package main

import (
	"math/rand"
	"fmt"
)

const (
	LOOPS = 1000
	DAYS = 365
)

func simulated(n int) float64 {
	sum := 0.0
	for k := 0; k < n; k++ {
		// Find probability of getting k distinct answers
		probs := make([]float64, LOOPS)
		// Find many
		for i := 0; i < LOOPS; i++ {
			dist := make([]int, DAYS)
			for i := 0; i < n; i++ {
				dist[rand.Intn(DAYS)]++
			}

			for _, d := range dist {
				// a distinct answer
				if d == 1 {
					probs[i]++
				}
			}
		}
		pr := 0.0
		for _, p := range probs {
			pr += p
		}
		sum += 1-(pr/float64(LOOPS * DAYS))
	}
	return sum
}

func main() {
		fmt.Printf("%2d: Simulated: %20v\n", DAYS, simulated(DAYS))
}
