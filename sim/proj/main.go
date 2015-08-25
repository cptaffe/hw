package main

import (
	"math/rand"
	"github.com/stanim/svgof/svgo2f"
	"os"
	"log"
	"fmt"
	"flag"
)

var maxMargin float64 = 0.5
var minMargin float64 = 0.5
var minDepth int = 50
var maxDepth int = 100

type Rect struct {
	X, Y float64
	Width, Height float64
	Right, Left *Rect
}

func (r *Rect) splitMaybe(depth int) {
	if depth > maxDepth {
		return
	}
	// 60% chance of descending
	if (rand.Float64() <= 0.5) {
		s := (rand.Float64() * (maxMargin-minMargin)) + minMargin
		if (rand.Intn(2) == 1) {
			// split horizontally
			s *= r.Height
			r.Right = &Rect { Width: r.Width, Height: s, X: r.X, Y: r.Y }
			r.Left = &Rect { Width: r.Width, Height: r.Height - s, X: r.X, Y: r.Y + s }
		} else {
			// split vertically
			s *= r.Width
			r.Right = &Rect { Width: s, Height: r.Height, X: r.X, Y: r.Y}
			r.Left = &Rect { Width: r.Width - s, Height: r.Height, X: r.X + s, Y: r.Y }
		}
		r.Right.splitMaybe(depth+1)
		r.Left.splitMaybe(depth+1)
	}
}

// Perhaps split the rectangle recursively
func (r *Rect) SplitMaybe() {
	r.splitMaybe(0)
}

func (r *Rect) Paint(canvas *svg.SVG) {
	if r.Right == nil && r.Left == nil {
		canvas.Rect(r.X, r.Y, r.Width, r.Height, fmt.Sprintf(`fill="rgb(%d, %d, %d)"`, rand.Intn(255), rand.Intn(255), rand.Intn(255)))
	} else {
		r.Right.Paint(canvas)
		r.Left.Paint(canvas)
	}
}

func (r *Rect) Depth() int {
	if r == nil {
		return 0
	}
	l := r.Left.Depth()
	rr := r.Right.Depth()
	if l > rr {
		return l+1
	} else {
		return rr+1
	}
}

func main() {
	var ip = flag.Int64("seed", 33, "Seeds random number generator")
	flag.Float64Var(&maxMargin, "maxmargin", maxMargin, "Maximum split margin")
	flag.Float64Var(&minMargin, "minmargin", minMargin, "Minimum split margin")
	flag.IntVar(&minDepth, "mindepth", minDepth, "Minumum recurse depth")
	flag.IntVar(&maxDepth, "maxdepth", maxDepth, "Maximum recurse depth")
	flag.Parse()

	rand.Seed(*ip)

	r := Rect { Width: 800, Height: 800 }
	for r.Depth() < minDepth {
		r.SplitMaybe()
	}

	file, err := os.Create("graph.svg")
	if err != nil {
		log.Fatal(err)
	}
	canvas := svg.New(file)
	canvas.Start(r.Width, r.Height)
	r.Paint(canvas)
	canvas.End()
}
