package main

import (
	"fmt"
	"io/ioutil" // Implements some I/O utility functions.
	// "bufio"
	"log"
	// "strings"
	// m "math"    // Math library with local alias m.
	// "os" // OS functions like working with the file system
	// "regexp"
	"strconv" // String conversions.
	"strings"
	// "sync"
)

var base = [4]int{0, 1, 0, -1}

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

	// Part 1
	// input := initialInput
  // for i := 0;  i<5; i++ {
  //   input = runPhase(input)
  // 	fmt.Printf("Phase %d: %+v\n", i, input)
  // }
	// fmt.Printf("After 100 Phases: %+v\n", input[:8])


	// Part 2
	input := make([]int, 0, len(initialInput) * 10000)
	offset, _ := strconv.Atoi(inputString[:7])

  for i := 0;  i<10000; i++ {
    input = append(input, initialInput...)
  }
	fmt.Printf("Initial length: %+v\n", len(initialInput))
	fmt.Printf("Offset: %+v\n", offset)
	fmt.Printf("Calc: %+v\n", len(input)-offset)
	fmt.Printf("Final length: %+v\n", len(input))
	// fmt.Printf("Initial %d: %+v\n", 0, input)

  for i := 0;  i<100; i++ {
    input = runPhaseTail(input, offset-10)
  	fmt.Printf("Phase %d: %+v\n", i, input[offset:offset+8])
  }
	fmt.Printf("After 100 Phases: %+v\n", input[offset:offset+8])
}


func getPattern(inputPos int, outputPos int) (int){
	repeat := outputPos*4
	pos := inputPos % repeat
	offset := pos / outputPos // int division uses floor
	return base[offset]
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

// For everything in the last half of the digits, the value for position P
// each iteration is output[P] = input[P] + input[P+1] + input[P+2]....
// this simplifies to input[P]+output[P+1]
func runPhaseTail(input []int, start int) ([]int) {
  output := make([]int, len(input))

  for i := 1;  i < len(input)-start; i++ {
		index := len(input) - i
		if (i == 1){
			output[index] = input[index]
		} else {
	    output[index] = input[index] + output[index+1]
		}
    if output[index] < 0 {
      output[index] *= -1
    }
    output[index] = output[index]  % 10
  }

  return output
}
