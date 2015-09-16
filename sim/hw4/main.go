package main

import (
	"math/rand"
	"github.com/stanim/svgof/svgo2f"
	"os"
	"log"
	"fmt"
)

const (
	LOOPS = 20000
	MAXPEOPLE = 50
)

func exact(people int) float64 {
	m := 1.0
	for i := 0; i < people; i++ {
		m *= (365.0 - float64(i))/365.0
	}
	return 1-m
}

func simulated(people int) float64 {
	probs := make([]int, LOOPS)
	for i := 0; i < LOOPS; i++ {
		dist := make([]int, 365)
		for i := 0; i < people; i++ {
			dist[rand.Intn(365)]++
		}

		for _, d := range dist {
			// at least a duplicate
			if d > 1 {
				probs[i]++
			}
		}
	}
	pr := 0
	for _, p := range probs {
		if p > 0 {
			pr++
		}
	}
	return float64(pr)/float64(LOOPS)
}

type Prob struct {
	Simulated float64
	Exact float64
}

func main() {
	p := make([]Prob, MAXPEOPLE)
	for i := 0; i < MAXPEOPLE; i++ {
		p[i] = Prob{ Simulated: simulated(i+1), Exact: exact(i+1) }
		fmt.Printf("%2d: Simulated: %20v, Exact: %20v\n", i+1, p[i].Simulated, p[i].Exact)
	}

	// Write svg graph
	width := 800.0
	height := 800.0
	hd := (height-10)/float64(1)
	wd := (width-10)/float64(MAXPEOPLE)
	file, err := os.Create("graph.svg")
	if err != nil {
		log.Fatal(err)
	}
	canvas := svg.New(file)
	canvas.Start(width, height)
	canvas.Rect(0, 0, width, height, `fill="beige"`)
	// Textual Key
	canvas.Text(5, 45, "Simulated", "text-anchor:right;font-size:16px;fill:red")
	canvas.Text(5, 25, "Exact", "text-anchor:right;font-size:16px;fill:blue")
	for i, w := range p {
		canvas.Circle((float64(i) * wd)+5, height - ((float64(w.Simulated) * hd)+5), 3, `fill="red"`)
		canvas.Circle((float64(i) * wd)+5, height - ((float64(w.Exact) * hd)+5), 3, `fill="blue"`)
	}
	canvas.End()
}
