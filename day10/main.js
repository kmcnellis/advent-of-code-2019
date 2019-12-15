var fs = require('fs');
var gcd = require( 'compute-gcd' );

fs.readFile('input.txt', 'utf8', function(err, contents) {
  console.log(contents)
  var map=contents.trim().split("\n").map(x => x.split(""))
  let point = maxSight(map.map(x=>x.slice()))
  // point = {x:3, y:8}
  console.log("point", point, map[point.x][point.y])
  destroy(map.map(x=>x.slice()), point)
});

var arrayToString = (array, pad)=>array.map(a=>a.map(b=>(typeof b === "string")?b:b.toFixed(pad)));

var print = (map, pad=2, digits=0)=>{
  map = arrayToString(map, digits)
  for (var x=0; x < map.length; x++){
    for (var y=0; y < map[x].length; y++){
      process.stdout.write(map[x][y].padStart(pad," "))
    }
    console.log()
  }
}

var maxSight = (map)=>{
  var counts=map.map(a=>a.map(b=>0))
  let max = 0;
  let point = {x:0, y:0};
  for (var x=0; x < map.length; x++){
    for (var y=0; y < map[x].length; y++){
      if (map[x][y] == "#"){
        let c = countSight(map.map(x=>x.slice()), {x:x,y:y});
        if (c > max) {
          max = c
          point = {x:x,y:y}
        }
        counts[x][y] = c
      }
      else{
        counts[x][y] = "."
      }
    }
  }
  print(counts, 6)
  console.log("max is", max)

  return point;
}

var countSight = (map,point)=>{
  // print(map,2)
  map[point.x][point.y] = "X"
  var angles = new Set()
  for (var x=0; x < map.length; x++){
    for (var y=0; y < map[x].length; y++){
      if (map[x][y] == '#'){
        angles.add(calcAngle(point,{x:x,y:y}))
      }
    }
  }
  return angles.size;
}

var destroy = (map, point) =>{
  var destroyed = 0;
  var angles = map.map(a=>a.map(b=>0))
  map[point.x][point.y] = "X"

  for (var x=0; x < map.length; x++){
    for (var y=0; y < map[x].length; y++){
      if (y==point.y && x==point.x) {
        angles[x][y] = "X"
      }
      else if (map[x][y] == '#'){
        angles[x][y] = calcAngle(point,{x:x,y:y});
      }
      else{
        angles[x][y] = "."
      }
    }
  }
  console.log("angles")
  print(angles, 6)
  console.log("map")
  print(map, 6)

  while (true){
    // One loop per 360 degree rotation
    var anglesSeen = new Set()
    for (var x=0; x < map.length; x++){
      for (var y=0; y < map[x].length; y++){
        if (map[x][y] == '#'){
          anglesSeen.add(angles[x][y])
        }
      }
    }
    // We've cleared the board
    if (anglesSeen.size == 0 ) break;

    // console.log("anglesSeen",anglesSeen.size,anglesSeen)

    while(anglesSeen.size > 0){
      // process.stdout.write(".")
      let next = Math.min(...Array.from(anglesSeen.values()));
      anglesSeen.delete(next)
      let minDist = Infinity
      let minPoint = {x:0,y:0}
      // get the closest point with that angle
      for (var x=0; x < map.length; x++){
        for (var y=0; y < map[x].length; y++){
          if (angles[x][y] === next){
            let dist = calcDistance(point,{x:x,y:y})
            if (dist < minDist){
              minDist = dist
              minPoint = {x:x,y:y}
            }
          }
        }
      }
      // Remove it
      // console.log("remove ",minPoint, minDist,next)
      angles[minPoint.x][minPoint.y] = ""
      map[minPoint.x][minPoint.y] = ""
      // console.log("angles")
      // print(angles, 6)
      // console.log("map")
      // print(map, 6)
      destroyed += 1
      if (destroyed == 200) console.log("200th point is ",minPoint)
    }

  }
}

var calcAngle = (origin,target)=>{
  let run = origin.x - target.x
  let rise = target.y - origin.y
  var angle = Math.atan2(rise, run) * 180 / Math.PI
  if (angle < 0) angle += 360
  return angle
}

var calcDistance = (origin,target)=>{
  let x = origin.x - target.x
  let y = origin.y - target.y
  return distance = Math.sqrt( x*x + y*y );
}
