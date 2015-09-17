package main

import (
	"math"
	"math/rand"
	"fmt"
	"container/heap"
)

// Hyper Space Position
type Pos struct {
	x, y, z float64
}

const (
	RHO = 10
	R = 28
	B = 8/3
	DELTATIME = 0.01
)

func (p *Pos) deltaX() float64 {
	return RHO * (p.y - p.x)
}

func (p *Pos) deltaY() float64 {
	return R * p.x - p.y - p.x * p.z
}

func (p *Pos) deltaZ() float64 {
	return p.x * p.y - B * p.z
}

func (p *Pos) Distance(o *Pos) float64 {
	dx := p.x - o.x
	dx *= dx
	dy := p.y - o.y
	dy *= dy
	dz := p.z - o.z
	dz *= dz
	return math.Sqrt(dx + dy + dz)
}

func (p *Pos) Update(deltaTime float64) *Pos {
	np := new(Pos)
	np.x = p.x + deltaTime * p.deltaX()
	np.y = p.y + deltaTime * p.deltaY()
	np.z = p.z + deltaTime * p.deltaZ()
	return np
}

func (p *Pos) String() string {
	return fmt.Sprintf("x: %f, y: %f, z: %f", p.x, p.y, p.z)
}

type HyperSpace struct {
	pos *Pos
	maxTime float64
	bits uint64
}

func (s *HyperSpace) String() string {
	return fmt.Sprintf("%s, T: %f", s.pos, s.maxTime)
}

func (s *HyperSpace) Simulate() *Pos {
	p := s.pos
	for i := 0.0; i < s.maxTime; i += DELTATIME {
		p = p.Update(DELTATIME);
	}
	return p
}

type Solution interface {
	// Cost of the current solution
	Cost() float64
	// Returns the next solution
	Next() (Solution, float64, int)
}

// Cost of the current hyperspace solution
func (s *HyperSpace) Cost() float64 {
	b := s.pos.Distance(&Pos{})
	s.Simulate()
	a := s.pos.Distance(&Pos{x: 18, y: 20, z: 45})
	return b + a
}

// Generate a neighborhood of solutions
func (s *HyperSpace) genNeighborhood() chan Solution {
	sc := make(chan Solution)
	go func() {
		for i := uint(0); i < 56; i++ {
			sc <-  HyperSpacefrombits(s.bits ^ (1 << i))
		}
		close(sc)
	}()
	return sc
}

type SolCost struct {
	sol Solution
	cost float64
}

type SolCostHeap []SolCost

func (h SolCostHeap) Len() int {
	return len(h)
}

func (h SolCostHeap) Less(i, j int) bool {
	return h[i].cost < h[j].cost
}

func (h SolCostHeap) Swap(i, j int) {
	h[i], h[j] = h[j], h[i]
}

func (h *SolCostHeap) Push(x interface{}) {
	*h = append(*h, x.(SolCost))
}

func (h *SolCostHeap) Pop() interface{} {
	old := *h
	n := len(old)
	x := old[n-1]
	*h = old[0:n-1]
	return x
}

// Generate a neighborhood of solutions
// and pick the best one. Returns nil on no
// better solution avaliable.
func (s *HyperSpace) Next() (Solution, float64, int) {
	h := &SolCostHeap{}
	heap.Init(h)
	for n := range s.genNeighborhood() {
		heap.Push(h, SolCost{sol: n, cost: n.Cost()})
	}
	sol := heap.Pop(h).(SolCost) // minimum cost
	fmt.Println("old:", s.Cost(), "new:", sol.cost)
	if sol.cost < s.Cost() {
		return sol.sol, sol.cost, 1
	} else {
		fmt.Println("here!")
		return nil, sol.cost, 1
	}
}

// Generate HyperSpace from random bits
func HyperSpacefrombits(n uint64) *HyperSpace {
	return &HyperSpace{
		pos: &Pos {
			x: float64((n & 0xffff0000000000) >> 40) / 0xffff * 40 - 20,
			y: float64((n & 0xffff000000) >> 24) / 0xffff * 60 - 30,
			z: float64((n & 0xffff00) >> 8) / 0xffff * 50,
		},
		maxTime: float64(n & 0xff) / 0xff * 5,
		bits: n,
	}
}

func RandomHyperSpace() Solution {
	return HyperSpacefrombits(uint64(rand.Intn(0x100000000000000)))
}

type SolutionGenerator func() Solution

func TestHyperSpace() Solution {
	return HyperSpacefrombits(0x8FED66B63E53DE)
}

func Search(maxRestarts int, sgen SolutionGenerator) (float64, int) {
	var bestCost float64
	var bestEvals int
	for s := 0; s < maxRestarts; s++ {
		sol := sgen()
		cost := sol.Cost()
		evals := 1
		for sol != nil {
			fmt.Println(sol.(*HyperSpace))
			sol, cost, evals = sol.Next()
		}
		bestCost = cost
		bestEvals = evals
	}
	return bestCost, bestEvals
}

func main() {
	c, e := Search(1, TestHyperSpace)
	fmt.Println("result:", c, e)
}
