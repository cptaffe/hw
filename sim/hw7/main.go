package main

import (
	"math/rand"
	"fmt"
	"github.com/stanim/svgof/svgo2f"
	"os"
	"log"
)

const (
	LOOPS = 10000
	SIDE = 20
	SPRINGSTEPSMAX = 100
	AIRPORTSTEPSMAX = 300
)

type Point struct {
	X int
	Y int
}

func (p *Point) Equals(o *Point) bool {
	return p.X == o.X && p.Y == o.Y
}

func (p *Point) Move() {
	r := rand.Intn(4)
	if r == 0 {
		p.X--
	} else if r == 1 {
		p.X++
	} else if r == 2 {
		p.Y--
	} else if r == 3 {
		p.Y++
	}
}

func (p *Point) Within(w, h int) bool {
	return p.X < w/2 && p.X > 0-w/2 && p.Y < h/2 && p.Y > 0-h/2
}

type DeathReason int

const (
	NOTDEAD DeathReason = iota
	DEATHBYDROWNING
	DEATHBYCANNIBALISM
	DEATHBYDEHYDRATION
	DEATHBYHUNGER
)

type Stat struct {
	Alive bool
	Reason DeathReason
	Steps int
}

var reasons []string = []string{
	NOTDEAD: "Not dead",
	DEATHBYDROWNING: "Drowned",
	DEATHBYCANNIBALISM: "Eaten by cannibals",
	DEATHBYDEHYDRATION: "Dehydrated",
	DEATHBYHUNGER: "Starved",
};

func journey() []Stat {
	cannibalVillage := &Point { X: 2, Y: -4 }
	spring := &Point { X: -3, Y: -3 }
	airport := &Point { X: 6, Y: 7 }
	results := make([]Stat, LOOPS)
	for i := 0; i < LOOPS; i++ {
		results[i] = func() Stat {
			lastSpringVisit := 0
			p := &Point { X: 0, Y: 0 }
			for steps := 0; steps < AIRPORTSTEPSMAX; steps++ {
				p.Move()
				if p.Equals(spring) {
					lastSpringVisit = steps
				} else if (steps-lastSpringVisit) >= SPRINGSTEPSMAX {
					return Stat { Alive: false, Reason: DEATHBYDEHYDRATION, Steps: steps }
				} else if !p.Within(SIDE, SIDE) {
					return Stat { Alive: false, Reason: DEATHBYDROWNING, Steps: steps }
				} else if p.Equals(cannibalVillage) {
					return Stat { Alive: false, Reason: DEATHBYCANNIBALISM, Steps: steps }
				} else if p.Equals(airport) {
					return Stat { Alive: true, Steps: steps }
				}
			}
			return Stat { Alive: false, Reason: DEATHBYHUNGER, Steps: AIRPORTSTEPSMAX }
		}()
	}
	return results
}

func main() {
	stats := journey()
	alives := make([]Stat, 0)
	length := 0.0
	longest := 0
	for _, s := range stats {
		if s.Alive {
			alives = append(alives, s)
			length += float64(s.Steps)
			if s.Steps > longest {
				longest = s.Steps
			}
		}
	}
	length /= float64(len(alives))
	fmt.Printf("Success rate: %v%%, Average length: %v\n", float64(len(alives))/float64(len(stats)) * 100, length)

	width := 800.0
	height := 800.0
	hd := (height-10)/float64(longest)
	wd := (width-10)/float64(len(alives))
	file, err := os.Create("graph.svg")
	if err != nil {
		log.Fatal(err)
	}
	canvas := svg.New(file)
	canvas.Start(width, height)
	canvas.Rect(0, 0, width, height, `fill="beige"`)
	canvas.Line(0, height - (length*hd)+5, width, height - (length*hd)+5, `stroke="red"`)
	for i, a := range alives {
		canvas.Circle((float64(i) * wd)+5, height - ((float64(a.Steps) * hd)+5), 2)
	}
	canvas.End()
}
