package main

import (
	"math/rand"
	"math"
	"fmt"
	"github.com/stanim/svgof/svgo2f"
	"os"
	"log"
)

const (
	DARTS = 30000
	LEFTEND = 2
	RIGHTEND = 4
	MINDIFF = 0.000001
	LOWERBOUND = 1
	AREA = 1
)

type Dart struct {
	x float64
	y float64
}

type Bounds struct {
	Left float64
	Right float64
	Darts []Dart
	last *Bounds
}

func (b *Bounds) Midpoint() float64 {
	return b.Left + (b.Right-b.Left)/2
}

func (b *Bounds) Enclose(offset float64) *Bounds {
	bn := &Bounds {
		Left: b.Left,
		Right: b.Right,
		last: b,
	}
	if offset > 0 {
		bn.Left = b.Midpoint()
	} else if offset < 0 {
		bn.Right = b.Midpoint()
	}
	return bn
}

func (b *Bounds) String() string {
	if b.last != nil {
		return fmt.Sprintf("{%f, %f}->%v", b.Right, b.Left, b.last)
	} else {
		return fmt.Sprintf("{%f, %f}", b.Right, b.Left)
	}
}

func f(x float64) float64 {
	return 1/x
}

func main() {
	bounds := &Bounds { Left: LEFTEND, Right: RIGHTEND }
	for math.Abs(bounds.Right-bounds.Left) >= MINDIFF {
		midpoint := bounds.Midpoint()

		bounds.Darts = make([]Dart, DARTS)
		// Throw lots of darts
		for i, _ := range bounds.Darts {
			bounds.Darts[i] = Dart{
				x: (rand.Float64() * (midpoint-LOWERBOUND)) + LOWERBOUND,
				y: rand.Float64() * f(LOWERBOUND),
			}
		}

		underCurve := float64(0)
		for _, d := range bounds.Darts {
			if d.y < 1/d.x {
				underCurve++
			}
		}

		area := AREA - ((underCurve/DARTS) * ((midpoint-LOWERBOUND) * f(LOWERBOUND)))
		bounds = bounds.Enclose(area)
	}
	fmt.Println(bounds);

	width := 800.0
	height := 800.0
	hd := (height-10)/f(LOWERBOUND)
	wd := (width-10)/(RIGHTEND-LOWERBOUND)
	count := 0
	dirName := "graphs"
	if err := os.RemoveAll(dirName); err != nil {
		log.Fatal(err)
	}
	if err := os.Mkdir(dirName, 0777); err != nil {
			log.Fatal(err)
	}
	for b := bounds.last; b != nil; b = b.last {
		file, err := os.Create(fmt.Sprintf("%s/graph_%.3d.svg", dirName, count))
		if err != nil {
			log.Fatal(err)
		}
		canvas := svg.New(file)
		canvas.Start(width, height)
		canvas.Rect(0, 0, width, height, `fill="beige"`)
		for _, d := range b.Darts {
			if d.y < f(d.x) {
				if d.x > b.Left {
					canvas.Circle(((d.x - LOWERBOUND)*wd)+5, height - ((d.y*hd)+5), 2, `fill="red"`)
				} else {
					canvas.Circle(((d.x - LOWERBOUND)*wd)+5, height - ((d.y*hd)+5), 2, `fill="blue"`)
				}
			} else {
				canvas.Circle(((d.x - LOWERBOUND)*wd)+5, height - ((d.y*hd)+5), 2)
			}
		}
		canvas.End()
		count++
	}
}
