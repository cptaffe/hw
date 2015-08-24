package main

import (
	"math/rand"
	"fmt"
	"github.com/stanim/svgof/svgo2f"
	"os"
	"log"
)

const (
	LOOPS = 100000
	PASSENGERS = 100
)

func main() {
	khasAssigned := make([]float64, PASSENGERS)
	for j := 0; j < LOOPS; j++ {
		chosen := make([]int, PASSENGERS)
		chosen[rand.Intn(PASSENGERS)] = 1 // plus one so 0 is empty
		for i := 1; i < len(chosen); i++ {
			if chosen[i] == 0 {
				// take assigned seat
				chosen[i] = i+1
			} else {
				// place in random empty spot
				r := rand.Intn(PASSENGERS)
				for {
					if chosen[r] == 0 {
						chosen[r] = i+1
						break
					}
					r = rand.Intn(PASSENGERS)
				}
			}
		}

		for i, _ := range chosen {
			if chosen[i]-1 == i {
				khasAssigned[i]++
			}
		}
	}

	for i, _ := range khasAssigned {
		khasAssigned[i] /= float64(LOOPS)
	}

	for i, k := range khasAssigned {
		exact := float64(PASSENGERS+1-(i+1))/float64(PASSENGERS+2-(i+1))
		fmt.Printf("%v: Exact: %v, Simulated: %v\n", i, exact, k)
	}

	width := 800.0
	height := 800.0
	hd := float64(height-10)
	wd := float64(width-10)/float64(len(khasAssigned))
	file, err := os.Create("graph.svg")
	if err != nil {
		log.Fatal(err)
	}
	canvas := svg.New(file)
	canvas.Start(width, height)
	canvas.Rect(0, 0, width, height, `fill="beige"`)
	for i, a := range khasAssigned {
		// TODO: draw line at avg
		canvas.Circle((float64(i) * wd)+5, height - ((float64(a) * hd)+5), 2)
	}
	canvas.End()
}
