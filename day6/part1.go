package main

import (
	"fmt"
	// "io/ioutil" // Implements some I/O utility functions.
	"bufio"
	"log"
	"strings"
	// "math"    // Math library with local alias m.
	"os" // OS functions like working with the file system
	// "strconv"   // String conversions.
)

type Planet struct {
	Name       string
	Orbits     int
	Parent     string
	HasInOrbit []string
}

func main() {
	// Setup planet map & com planet
	planets := make(map[string]*Planet)

	com := &Planet{Name: "COM", Parent: "", HasInOrbit: make([]string, 0)}
	planets["COM"] = com

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
		// fmt.Println(o)
		// Split into planet and parent (orbited) planet
		curr := strings.Split(o, `)`)
		parentName := curr[0]
		planetName := curr[1]

		// Save planet to map
		planets[planetName] = &Planet{Name: planetName, Parent: parentName, HasInOrbit: make([]string, 0)}

		// fmt.Println(parentName, "orbits", planetName)
	}

	// For each planet, add it to it's parent's planets in orbit
	for _, p := range planets {
		// fmt.Printf("planet: %+v\n", p)
		if parent, ok := planets[p.Parent]; ok {
			// fmt.Println("found parent")

			parent.HasInOrbit = append(parent.HasInOrbit, p.Name)
			planets[p.Parent] = parent
			// fmt.Printf("parent: %+v\n", parent)
		}
	}

	// Recursivly count the number of orbits for each planet
	countOrbits(planets, "COM")

	// Count the total number of orbits
	count := 0
	for _, i := range planets {
		count += i.Orbits
		fmt.Printf("planets: %+v\n", i)
	}
	fmt.Println("count", count)

}

func countOrbits(planets map[string]*Planet, p string) {
	// fmt.Println("count", p)

	planet := planets[p]
	// fmt.Printf("planet: %+v\n", planet)
	if p == "COM" {
		planet.Orbits = 0
	} else {
		// The orbit count is equal to parent's orbits (indeirect) +1 (direct)
		parent := planets[planet.Parent]
		planet.Orbits = parent.Orbits + 1
	}

	// Now that we have our current planet's orbits counted, count for each
	// child
	for _, i := range planet.HasInOrbit {
		countOrbits(planets, i)
	}
}
