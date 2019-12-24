package main

import (
	"fmt"
	"io/ioutil" // Implements some I/O utility functions.
	// "bufio"
	"log"
	// "strings"
	// "math"    // Math library with local alias m.
	// "os" // OS functions like working with the file system
	// "regexp"
	"strconv" // String conversions.
	"strings"
	// "math"
)


func main() {
	// Open input file
	// Read in file
  dat, err := ioutil.ReadFile("input.txt")
	if err != nil {
		log.Fatalf("failed opening file: %s", err)
	}
  inputString := strings.TrimSuffix(string(dat), "\n")

  initialInput := make([]int, len(inputString))

  for i, item := range inputString {
    initialInput[i], _ = strconv.Atoi(string(item))
  }

	fmt.Printf("Initial: %+v\n", initialInput)

  input := initialInput

  for i := 0;  i<100; i++ {
    input = runPhase(input)
  	// fmt.Printf("Phase %d: %+v\n", i, input)
  }
	fmt.Printf("After 100 Phases: %+v\n", input[:8])

}

func generatePattern(outputNum int, length int) ([]int){
  pattern := make([]int, length+1)
  base := [4]int{0, 1, 0, -1}

  basePart := 0
  baseNum := 0
  for i := 0;  i<length+1; i++ {
    pattern[i] = base[basePart]
    baseNum+=1
    if baseNum >= outputNum{
      basePart += 1
      basePart = basePart % 4
      baseNum = 0
    }
  }

  return pattern[1:]
}

func runPhase(input []int) ([]int) {
  output := make([]int, len(input))

  for i := 0;  i<len(input); i++ {
    pattern := generatePattern(i+1, len(input))
    // fmt.Printf("Pattern %d: %+v\n", i, pattern)
    output[i] = 0
    for pos := 0; pos < len(input); pos++{
    	// fmt.Printf("%d*%d  +  ", input[pos] , pattern[pos])

      output[i] += input[pos] * pattern[pos]
    }
    if output[i] < 0 {
      output[i] *= -1
    }
    output[i] = output[i]  % 10
  	// fmt.Printf("= %d\n", output[i])

  }

  return output
}