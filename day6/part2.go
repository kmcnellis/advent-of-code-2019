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
	Jumps      int
	Visited    bool
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

	myPlanet := planets["YOU"].Parent
	santaPlanet := planets["SAN"].Parent

	// Recursivly count the number of orbits for each planet
	jumps := searchOrbits(planets, myPlanet, santaPlanet)

	// Count the total number of orbits
	// count := 0
	for _, i := range planets {
		// count += i.Orbits
		fmt.Printf("planets: %+v\n", i)
	}
	fmt.Println("jumps", jumps)

}

func searchOrbits(planets map[string]*Planet, p string, goal string) int{
	// fmt.Println("count", p)

	// Setup
	var queue []string
	current := planets[p]

	for current.Name != goal {

		// Add parent to the queue
		if current.Parent != "" && !planets[current.Parent].Visited {
			queue = append(queue, current.Parent)
			planets[current.Parent].Visited = true
			planets[current.Parent].Jumps = current.Jumps + 1
		}
		// Add children to the queue
		for _, i := range current.HasInOrbit {
			if i != "" && !planets[i].Visited {
				queue = append(queue, i)
				planets[i].Visited = true
				planets[i].Jumps = current.Jumps + 1
			}
		}

		current = planets[queue[0]]
		queue = queue[1:] // Dequeue

	}
	return current.Jumps

}
