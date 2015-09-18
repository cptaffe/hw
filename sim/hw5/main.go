package main

/*
*
* Homework 5
* CPSC 4399-11; Special Topics: Monte Carlo Simulation
* Professor Minsker
* Connor P. Taffe
* T no. 00563742
*
*/

import (
	"math/rand"
	"fmt"
)

const (
	DAYS = 365
	LOOPS = 100000
)

func distribution() int {
	dist := make([]int, DAYS)
	d := 0

	for _, _ = range dist {
		r := rand.Intn(DAYS)
		if dist[r] == 0 {
			d++
		} else if dist[r] == 1 {
			d--
		}
		dist[r]++
	}
	return d
}

func sim() float64 {
	s := 0
	for i := 0; i < LOOPS; i++ {
		s += distribution()
	}
	return DAYS-float64(s)/LOOPS
}

func main() {
	fmt.Println("Simulation: ", sim())
}
