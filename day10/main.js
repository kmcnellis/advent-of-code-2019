var fs = require('fs');
var gcd = require( 'compute-gcd' );

fs.readFile('test1.txt', 'utf8', function(err, contents) {
    console.log(contents)
    var map=contents.trim().split("\n").map(x => x.split(""))
    var counts=contents.trim().split("\n").map(x => x.split(""))
    console.log(map)
    for (var y=0; y < map.length; y++){
      for (var x=0; x < map[y].length; x++){
        if (map[x][y] == "#"){
          let c = countSight(map.map(x=>x.slice()), {x:x,y:y});
          counts[x][y] = c
        }
        else{
          counts[x][y] = 0
        }
      }
    }

    console.log(map)
    console.log(counts)
});

var print = (map)=>{
    for (var y=0; j < map.length; j++){
      for (var x=0; i < map[y].length; i++){
        process.stdout.write(map[x][y])
      }
      console.log()
    }
    return c;
}


var countSight = (map,point)=>{
    map[point.x][point.y] = "X"
    console.log("countSight")
    console.log(point)
    console.log(map)
  let c = 0;
    for (var y=0; y < map.length; y++){
      for (var x=0; x < map[y].length; x++){
        if (map[x][y] == '#' && x!=point.x && y!=point.y){
          map[x][y] = "!";

          console.log("found", x,y)
          console.log(map)
          c+=1;
          let s = slope(point, {x:x,y:y})
          console.log("slope", s)
          a += s.x;
          b += s.y;
          console.log("look", a,b)
          for (var a = point.x, b = point.y; ; a < map.length && b < map[a].length && a >= 0 && b >= 0){
            if (a == point.x && b == point.y) break;
            console.log("intersect", a,b)
            map[a][b] = " ";
            console.log(map)
            a += s.x;
            b += s.y;
            console.log("look", a,b)
          }
        }
      }
    }
    return c;
}

var slope = (a,b)=>{
  let x = b.x - a.x
  let y = b.y - a.y
  var d = gcd( x, y );
  console.log("slope find", a,b, d)
  x = x/d
  y = y/d

  return {x:x, y:y}
}
