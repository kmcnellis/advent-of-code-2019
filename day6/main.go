package main

import (
    "fmt"
    "io/ioutil" // Implements some I/O utility functions.
    "strings"
    // "bufio"
    // "math"    // Math library with local alias m.
    // "os"        // OS functions like working with the file system
    // "strconv"   // String conversions.
)

func main() {
    // Println outputs a line to stdout.
    // It comes from the package fmt.
    fmt.Println("Hello world!")
    dat, _ := ioutil.ReadFile("test.txt")
    orbits := strings.Split(string(dat),`\n`)

    for o := range orbits {
      fmt.Println(string(o))
      fmt.Println("string")
    }
}
