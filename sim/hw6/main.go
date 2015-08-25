package main

import (
	"math/rand"
	"sort"
	"github.com/stanim/svgof/svgo2f"
	"os"
	"log"
	"fmt"
	"runtime"
)

const (
	LOOPS = 100000
	LOWERPEDESTALBOUND = 2
	UPPERPEDESTALBOUND = 20+1
)

type Stat struct {
	Index int
	Wins int
}

type StatArray struct {
	s []Stat
}

func (s *StatArray) Len() int {
	return len(s.s);
}

func (s *StatArray) Less(i, j int) bool {
	return s.s[i].Wins > s.s[j].Wins
}

func (s *StatArray) Swap(i, j int) {
	s.s[i], s.s[j] = s.s[j], s.s[i]
}

func main() {
	results := make([]StatArray, UPPERPEDESTALBOUND-LOWERPEDESTALBOUND)
	schan := make(chan StatArray)
	for p := LOWERPEDESTALBOUND; p < UPPERPEDESTALBOUND; p++ {
		wchan := make(chan int)
		for i := 0; i < runtime.NumCPU(); i++ {
			jump := func(ped, nped []int, i int) {
				if (ped[i] != 0) {
					j := i - (rand.Intn(2))
					if j < 0 {
						j = 0
					} else if j >= len(nped) {
						j = len(nped)-1
					}
					if nped[j] == 0 || (rand.Intn(2)) == 0 {
						nped[j] = ped[i]
					}
				}
			}

			max := LOOPS/runtime.NumCPU()
			if i == runtime.NumCPU()-1 {
				max += LOOPS%runtime.NumCPU()
			}

			go func(w chan int, max, p int) {
				for i := 0; i < max; i++ {
					ped := make([]int, p)
					for i, _ := range ped {
						ped[i] = i+1 // avoid value 0, 0 is dead.
					}
					for len(ped) > 1 {
						nped := make([]int, len(ped)-1)
						for i := 0; i < len(ped); i++ {
							// Only jump filled pedastles
							jump(ped, nped, i)
						}
						ped = nped
					}
					w<- ped[0]-1 // counter offset
				}
			}(wchan, max, p)
		}

		go func(w chan int, p int) {
			s := StatArray{ s: make([]Stat, p) }
			for i := 0; i < LOOPS; i++ {
				index := <-w
				s.s[index].Index = index
				s.s[index].Wins++
			}
			sort.Sort(&s)
			schan<- s
		}(wchan, p)
	}

	for i := LOWERPEDESTALBOUND; i < UPPERPEDESTALBOUND; i++ {
		s := <-schan
		results[len(s.s)-LOWERPEDESTALBOUND] = s
	}

	for p := LOWERPEDESTALBOUND; p < UPPERPEDESTALBOUND; p++ {
		// Enumerate results
		fmt.Printf("== Running with %v pedastles ==\n", p)
		for _, w := range results[p-LOWERPEDESTALBOUND].s {
			fmt.Printf("Index: %v, Wins: %v\n", w.Index, w.Wins)
		}
	}

	// Write svg graphs
	dirName := "graphs"
	if err := os.RemoveAll(dirName); err != nil {
		log.Fatal(err)
	}
	if err := os.Mkdir(dirName, 0777); err != nil {
			log.Fatal(err)
	}
	for p := LOWERPEDESTALBOUND; p < UPPERPEDESTALBOUND; p++ {
		largest := 0
		for _, w := range results[p-LOWERPEDESTALBOUND].s {
			if w.Wins > largest {
				largest = w.Wins
			}
		}

		width := 800.0
		height := 800.0
		hd := (height-10)/float64(largest)
		wd := (width-10)/float64(p-1)
		file, err := os.Create(fmt.Sprintf("%s/graph_%.2d.svg", dirName, p))
		if err != nil {
			log.Fatal(err)
		}
		canvas := svg.New(file)
		canvas.Start(width, height)
		canvas.Rect(0, 0, width, height, `fill="beige"`)
		for _, w := range results[p-LOWERPEDESTALBOUND].s {
			canvas.Circle((float64(w.Index) * wd)+5, height - ((float64(w.Wins) * hd)+5), 2)
		}
		canvas.End()
	}
}
