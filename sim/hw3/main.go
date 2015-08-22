package main

import (
	"math/rand"
	"fmt"
	"github.com/ajstarks/svgo"
	"os"
	"log"
)

const (
	LOOPS = 100000
	DECKSIZE = 52
)

func main() {
	matches := make([]int, LOOPS)
	for l := 0; l < LOOPS; l++ {
		var decks [2][]int

		for i, _ := range decks {
			decks[i] = make([]int, DECKSIZE)
			deck := decks[i]
			for i, _ := range deck {
				deck[i] = i
			}
			for i, _ := range deck {
				j := rand.Intn(DECKSIZE)
				deck[i], deck[j] = deck[j], deck[i]
			}
		}

		for i := 0; i < DECKSIZE; i++ {
			if decks[0][i] == decks[1][i] {
				matches[l]++
			}
		}
	}

	nmatches := 0
	for _, m := range matches {
		if m > 0 {
			nmatches++
		}
	}
	fmt.Printf("Probability of at least one match: %v\n", float64(nmatches)/float64(LOOPS))

	iprob := make([]int, DECKSIZE)
	for _, m := range matches {
		iprob[m]++
	}

	width := 800
	height := 800
	hd := float64(height-10)/float64(DECKSIZE)
	wd := float64(width-10)/float64(LOOPS)
	file, err := os.Create("graph.svg")
	if err != nil {
		log.Fatal(err)
	}
	canvas := svg.New(file)
	canvas.Start(width, height)
	canvas.Rect(0, 0, width, height, `fill="beige"`)
	for i, m := range matches {
		canvas.Circle(int(float64(i)*wd)+5, height - (int(float64(m)*hd)+5), 2)
	}

	largest := 0
	for _, p := range iprob {
		if p > largest {
			largest = p
		}
	}

	hd = float64(height-10)/float64(DECKSIZE)
	wd = float64(width-10)/float64(largest)

	for i, p := range iprob {
		if p > 0 {
			canvas.Circle(int(float64(p)*wd)+5, height - (int(float64(i)*hd)+5), 4, `fill="red"`)
		}
	}
	canvas.End()
}
