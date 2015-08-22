package main

import (
	"math/rand"
	"fmt"
	"github.com/ajstarks/svgo"
	"os"
	"log"
)

const (
	LOOPS = 10000
	SIDE = 20
	PIZZAHUTSTEPSMAX = 60
	AIRPORTSTEPSMAX = 80
)

type Point struct {
	X int
	Y int
}

func (p *Point) Equals(o *Point) bool {
	return p.X == o.X && p.Y == o.Y
}

func (p *Point) Move() {
	for {
		np := &Point { X: p.X, Y: p.Y }
		r := rand.Intn(4)
		if r == 0 {
			np.X--
		} else if r == 1 {
			np.X++
		} else if r == 2 {
			np.Y--
		} else if r == 3 {
			np.Y++
		}

		// Choose again if in lake or outside island
		if np.Within(0-SIDE/2, 0-SIDE/2, SIDE/2, SIDE/2) && !np.Within(4, 5, 9, 9) {
			*p = *np
			return
		}
	}
}

func (p *Point) Within(x, y, x1, y1 int) bool {
	return p.X < x1 && p.X >= x && p.Y < y1 && p.Y >= y
}

type DeathReason int

const (
	NOTDEAD DeathReason = iota
	DEATHBYDROWNING
	DEATHBYCANNIBALISM
	DEATHBYHUNGER
	DEATHBYBOREDOM
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
	DEATHBYHUNGER: "Starved",
	DEATHBYBOREDOM: "Died of boredom",
};

func journey() []Stat {
	bank := &Point { X: -5, Y: -2 }
	pizzaHut := &Point { X: -3, Y: -5 }
	airport := &Point { X: 9, Y: -2 }
	results := make([]Stat, LOOPS)
	for i := 0; i < LOOPS; i++ {
		results[i] = func() Stat {
			lastPizzaHutVisit := 0
			hasMoney := false
			p := &Point { X: 0, Y: 0 }
			for steps := 0; steps < AIRPORTSTEPSMAX; steps++ {
				p.Move()
				if (steps-lastPizzaHutVisit) >= PIZZAHUTSTEPSMAX {
					return Stat { Alive: false, Reason: DEATHBYHUNGER, Steps: steps }
				} else if p.Equals(bank) {
					hasMoney = true
				} else if hasMoney && p.Equals(pizzaHut) {
					lastPizzaHutVisit = steps
					hasMoney = false
				} else if p.Equals(airport) {
					return Stat { Alive: true, Steps: steps }
				}
			}
			return Stat { Alive: false, Reason: DEATHBYBOREDOM, Steps: AIRPORTSTEPSMAX }
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

	width := 800
	height := 800
	hd := float64(height-10)/float64(longest)
	wd := float64(width-10)/float64(len(alives))
	file, err := os.Create("graph.svg")
	if err != nil {
		log.Fatal(err)
	}
	canvas := svg.New(file)
	canvas.Start(width, height)
	canvas.Rect(0, 0, width, height, `fill="beige"`)
	canvas.Line(0, height - int(length*hd)+5, width, height - int(length*hd)+5, `stroke="red"`)
	for i, a := range alives {
		// TODO: draw line at avg
		canvas.Circle(int(float64(i) * wd)+5, height - (int(float64(a.Steps) * hd)+5), 2)
	}
	canvas.End()
}
