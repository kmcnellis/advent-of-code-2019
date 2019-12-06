var fs = require('fs');

fs.readFile('input.txt', 'utf8', function(err, contents) {
    var massList=contents.trim().split("\n").map(x => parseFloat(x))
    console.log(massList)
    var totalFuel = massList.map(x=>{
      var fuel = calcFuel(x)
      var extraFuel = calcFuel(fuel)
      while (extraFuel > 0){
        // console.log("extraFuel",extraFuel);
        fuel += extraFuel
        extraFuel = calcFuel(extraFuel)
      }
      return fuel
    }).reduce((sum,val)=>sum+val)
    console.log("total",totalFuel);

});

var calcFuel = (mass)=>{
  var fuel = Math.floor(mass/3)-2
  if (fuel > 0) return fuel
  else return 0
}
