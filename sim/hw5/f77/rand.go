package rand

import (
	"C"
	"math/rand"
)

// export rand_
func rand_() C.double {
	return C.double(rand.Float64())
}
