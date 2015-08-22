package main

import (
	"math/rand"
	"fmt"
	"github.com/ajstarks/svgo"
	"os"
	"log"
)

const (
	DARTS = 100000
)

type Dart struct {
	x float64
	y float64
}

func main() {
	darts := make([]Dart, DARTS)
	for i := 0; i < DARTS; i++ {
		darts[i] = Dart{ x: rand.Float64(), y: rand.Float64() }
	}
	inCircle := float64(0)
	for _, d := range darts {
		if ((d.x * d.x) + (d.y * d.y)) <= 1.0 {
			inCircle++
		}
	}
	fmt.Printf("%f\n", (4 * inCircle)/DARTS)

	width := 800
	height := 800
	hd := float64(height-10)
	wd := float64(width-10)
	file, err := os.Create("graph.svg")
	if err != nil {
		log.Fatal(err)
	}
	canvas := svg.New(file)
	canvas.Start(width, height)
	canvas.Rect(0, 0, width, height, `fill="beige"`)
	for _, d := range darts {
		if ((d.x * d.x) + (d.y * d.y)) <= 1.0 {
			canvas.Circle(int(float64(d.x) * wd)+5, height - (int(float64(d.y) * hd)+5), 2, "fill=\"blue\"")
		} else {
			canvas.Circle(int(float64(d.x) * wd)+5, height - (int(float64(d.y) * hd)+5), 2)
		}
	}
	canvas.End()
}
