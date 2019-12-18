package main

import (
	"fmt"
	// "io/ioutil" // Implements some I/O utility functions.
	"bufio"
	"log"
	// "strings"
	"math" // Math library with local alias m.
	"os"   // OS functions like working with the file system
	"regexp"
	"strconv" // String conversions.
)

//
type Ingredient struct {
	name   string
	amount int
}

type Request struct {
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

	// Open input file
	file, err := os.Open("input.txt")
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
	producers := make(map[string]chan Request)
	recivers := make(map[string]chan Ingredient)
	producers["ORE"] = make(chan Request, 100)
	recivers["PROGRAM"] = make(chan Ingredient, 100)
	finished := make(chan bool)

	oreCount := 1000000000000
	fuelCount := 0
	for _, r := range recipes {
		producers[r.produces.name] = make(chan Request, 100)
		recivers[r.produces.name] = make(chan Ingredient, 100)
	}
	for _, r := range recipes {
		go func(r Recipe) {
			have := 0
			for true {
				request := <-producers[r.produces.name]
				// fmt.Println("Requested", r.produces.name, request.amount, "for", request.name)
				if have >= request.amount {
					// We have enough left over
					have -= request.amount
					// fmt.Println("Give", r.produces.name, request.amount, "to", request.name)
					recivers[request.name] <- Ingredient{name: r.produces.name, amount: request.amount}
					// fmt.Println("Have", r.produces.name, have)
				} else {
					// We need more
					need := int(math.Ceil(float64(request.amount-have) / float64(r.produces.amount)))
					// fmt.Println("Making", r.produces.name, need*r.produces.amount, "for", request.name, "have", have)

					// Get our recipe
					for _, item := range r.needs {
						// fmt.Println(r.produces.name, "Ask", item.name, item.amount*need)

						producers[item.name] <- Request{name: r.produces.name, amount: item.amount * need} // send request
						// _ = <-recivers[item.name]    // wait for response
					}
					// Collect ingrediants
					for _, _ = range r.needs {
						item := <-recivers[r.produces.name] // wait for responses
						if item.amount == 0 {
							// We can't get it, return
							// fmt.Println("Out of ", item.name, "in", request.name)
							recivers[request.name] <- Ingredient{name: r.produces.name, amount: 0}
							return
						}
					}

					// Have everything, increment have
					have += r.produces.amount * need
					// Send one out
					have -= request.amount
					// fmt.Println("Give", r.produces.name, request.amount, "to", request.name)
					recivers[request.name] <- Ingredient{name: r.produces.name, amount: request.amount}
					// fmt.Println("Have", r.produces.name, have)
				}
			}
		}(r)
	}
	go func() {
		for true {
			request := <-producers["ORE"]
			if oreCount >= request.amount {
				oreCount -= request.amount
				// fmt.Println("Give ORE", request.amount, oreCount, "to", request.name)
				recivers[request.name] <- Ingredient{name: "ORE", amount: request.amount}
			} else {
				// Out of ore
				// fmt.Println("Out of ore", request.amount, oreCount, "to", request.name)
				// We can't get it
				recivers[request.name] <- Ingredient{name: "ORE", amount: 0}
				finished <- true
			}
		}
	}()
	processing := true
	for processing {
		// fmt.Println("NEW FUEL START", fuelCount)
		producers["FUEL"] <- Request{name: "PROGRAM", amount: 1} // send request
		select {
		case fuel := <-recivers["PROGRAM"]:
			// fmt.Println("get fuel", fuel)
			// We recived a fuel
			fuelCount += fuel.amount
			// fmt.Println("Made fuel", fuelCount)
			if fuelCount%10000 == 0 {
				fmt.Println("Made fuel", fuelCount)
			}

		case _ = <-finished:
			processing = false;
		}
	}
	fmt.Println("DONE")
	fmt.Println("Fuel Count", fuelCount)

}

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
