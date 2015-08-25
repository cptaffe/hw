package main

import (
	"math/rand"
	"github.com/stanim/svgof/svgo2f"
	"os"
	"log"
	"fmt"
	"flag"
)

var maxMargin, minMargin float64 = 0.5, 0.5
var minDepth, maxDepth int = 10, 100
var splitChance, horizontalChance float64 = 0.5, 0.5
var lighterAsDescends, darkerAsDescends bool = false, false

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
	if (rand.Float64() < splitChance) {
		s := (rand.Float64() * (maxMargin-minMargin)) + minMargin
		if (rand.Float64() < horizontalChance) {
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

func (r *Rect) Paint(canvas *svg.SVG, cdepth int, depth int) {
	if r.Right == nil && r.Left == nil {
		var scale float64
		if lighterAsDescends {
			scale = float64(cdepth)/float64(depth) * 100
		} else if darkerAsDescends {
			scale = float64(depth-cdepth)/float64(depth) * 100
		} else {
			scale = 100
		}
		canvas.Rect(r.X, r.Y, r.Width, r.Height, fmt.Sprintf(`fill="rgb(%v%%, %v%%, %v%%)"`, rand.Float64() * scale, rand.Float64() * scale, rand.Float64() * scale))
	} else {
		r.Right.Paint(canvas, cdepth+1, depth)
		r.Left.Paint(canvas, cdepth+1, depth)
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
	flag.Float64Var(&splitChance, "splitchance", splitChance, "Chance of splitting on a pane")
	flag.Float64Var(&horizontalChance, "horizontalchance", horizontalChance, "Chance of splitting horizontally on a pane")
	flag.BoolVar(&lighterAsDescends, "lighterasdescends", lighterAsDescends, "Image is lighter to depict depth")
	flag.BoolVar(&darkerAsDescends, "darkerasdescends", darkerAsDescends, "Image is darker to depict depth")
	flag.Parse()

	// sanity checks
	if minMargin > maxMargin {
		log.Fatal("mimmargin must be less than maxmargin")
	}
	if minDepth > maxDepth {
		log.Fatal("mindepth must be less than maxdepth")
	}

	rand.Seed(*ip)

	r := Rect { Width: 800, Height: 800 }
	for r.Depth() < minDepth {
		r.SplitMaybe()
	}
	depth := r.Depth()

	file, err := os.Create("graph.svg")
	if err != nil {
		log.Fatal(err)
	}
	canvas := svg.New(file)
	canvas.Start(r.Width, r.Height)
	r.Paint(canvas, 0, depth)
	canvas.End()
}
