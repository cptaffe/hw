package main

import (
	"math/rand"
	"sort"
	"github.com/ajstarks/svgo"
	"os"
	"log"
	"fmt"
)

const (
	LOOPS = 25000
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
	for PEDESTALES := 2; PEDESTALES < 21; PEDESTALES++ {
		wchan := make(chan int)
		for i := 0; i < 8; i++ {
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

			go func() {
				for i := 0; i < LOOPS/8+1; i++ {
					ped := make([]int, PEDESTALES)
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
					wchan<- ped[0]-1 // counter offset
				}
			}()
		}

		s := StatArray{ s: make([]Stat, PEDESTALES) }
		for i := 0; i < LOOPS; i++ {
			index := <-wchan
			s.s[index].Index = index
			s.s[index].Wins++
		}

		sort.Sort(&s)

		// Enumerate results
		fmt.Printf("== Running with %v pedastles ==\n", PEDESTALES)
		largest := 0
		for _, w := range s.s {
			if w.Wins > largest {
				largest = w.Wins
			}
			fmt.Printf("Index: %v, Wins: %v\n", w.Index, w.Wins)
		}

		// Write svg graph
		width := 800
		height := 800
		hd := float64(height-10)/float64(largest)
		wd := float64(width-10)/float64(PEDESTALES)
		file, err := os.Create(fmt.Sprintf("graph_%.2d.svg", PEDESTALES))
		if err != nil {
			log.Fatal(err)
		}
		canvas := svg.New(file)
		canvas.Start(width, height)
		canvas.Rect(0, 0, width, height, `fill="beige"`)
		for _, w := range s.s {
			canvas.Circle(int(float64(w.Index) * wd)+5, height - (int(float64(w.Wins) * hd)+5), 2)
		}
		canvas.End()
	}
}
