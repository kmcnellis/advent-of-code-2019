package main

import (
	"fmt"
	// "io/ioutil" // Implements some I/O utility functions.
	"bufio"
	"log"
	// "strings"
	// "math"    // Math library with local alias m.
	"os" // OS functions like working with the file system
	"regexp"
	"strconv" // String conversions.
)

//
type Ingredient struct {
	name   string
	amount int
}

type Recipe struct {
	produces Ingredient
	needs    []Ingredient
}

func main() {
	// Setup planet map & com planet
	recipes := make(map[string]Recipe)
	// have := make(map[string]Ingredient)

	// Open input file
	file, err := os.Open("test4.txt")
	if err != nil {
		log.Fatalf("failed opening file: %s", err)
	}
	defer file.Close()

	// Split the file on newlines
	scanner := bufio.NewScanner(file)
	scanner.Split(bufio.ScanLines)

	// Read in file
	for scanner.Scan() {
		o := scanner.Text()
		r, err := parseInputLine(o)
		if err != nil {
			fmt.Println("Error, exiting")
			os.Exit(1)
		}
		recipes[r.produces.name] = r
	}
	for _, r := range recipes {
		fmt.Printf("%+v\n", r)
	}
	producers := make(map[string]chan bool)
	recivers := make(map[string]chan bool)
	producers["ORE"] = make(chan bool)
	recivers["ORE"] = make(chan bool)

	oreCount := 0
	for _, r := range recipes {
		producers[r.produces.name] = make(chan bool)
		recivers[r.produces.name] = make(chan bool)
	}
	for _, r := range recipes {
		go func(r Recipe) {
			// fmt.Println("Start", r.produces.name)
			have := 0
			for true {
				_ = <-producers[r.produces.name]
				// fmt.Println("Requested", r.produces.name)
				if have > 0 {
					// We have some left over
					// fmt.Println("Giving", r.produces.name)
					have -= 1
					recivers[r.produces.name] <- true // send what we have
				} else {
					// We need more
					// fmt.Println("Making", r.produces.name)
					// Get our recipe
					for _, item := range r.needs {
						// fmt.Printf("need: %+v\n", item.name)
						// fmt.Println("Don't have", item.name)
						for getting := 0; getting < item.amount; getting++ {
							producers[item.name] <- true // send request
							_ = <-recivers[item.name]    // wait for response
						}
					}
					// Have everything, increment have
					have += r.produces.amount
					// Send one out
					// fmt.Println("Giving", r.produces.name)
					have -= 1
					recivers[r.produces.name] <- true
				}
			}
		}(r)
		go func() {
			for true {
				_ = <-producers["ORE"]
				oreCount += 1
				// fmt.Println("Asked for ORE", count)
				recivers["ORE"] <- true
			}
		}()

	}
	producers["FUEL"] <- true
	done := <-recivers["FUEL"]
	fmt.Println("DONE", done)
	fmt.Println("Ore Count", oreCount)

	// // fmt.Printf("Recipes: %+v\n", recipes)
	// requestOne := make(chan bool)
	// go func() {
	//   b = a * b
	//
	//   operationDone <- true
	// }()
	//
	// <-operationDone
	//
	//
	// makeItem(Ingredient{name:"FUEL", amount:1}, have, recipes)

}

// func makeItem(item Ingredient, have map[string]Ingredient, recipes map[string]Recipe) {
// 	need := recipes[item.name].needs
// 	fmt.Printf("looking: %+v\n", need)
// 	for _, n := range need {
// 		if n.name == "ORE" || have[n.name].amount > n.amount {
// 			fmt.Println("Have", n.name)
//
// 		} else {
// 			fmt.Println("Don't have", n.name)
// 			makeItem(n, have, recipes)
// 		}
// 	}
// }

func parseInputLine(text string) (Recipe, error) {
	lineRe := regexp.MustCompile(`\d{1,} [A-Z]{1,}`)
	// lineRe := regexp.MustCompile(`^(\d* [A-Z]*,?)* => (\d* [A-Z]*)$`)
	matches := lineRe.FindAllString(text, -1)
	var items []Ingredient
	for _, item := range matches {

		ingRe := regexp.MustCompile(`\d{1,}|[A-Z]{1,}`)
		ingMatches := ingRe.FindAllString(item, -1)
		amount, err := strconv.Atoi(ingMatches[0])
		if err != nil {
			fmt.Println(err)
			var r Recipe
			return r, err
		}
		ing := Ingredient{
			name:   ingMatches[1],
			amount: amount}
		items = append(items, ing)
	}

	r := Recipe{produces: items[len(items)-1], needs: items[:len(items)-1]}
	return r, nil
}
