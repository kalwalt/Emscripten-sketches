function init() {
  Module.addPoints(2, 6);
  //Module.readVec();
  let points = new Module.vectorPoint2d();
  let p = points.size();
  console.log(p);
  points.resize(1,[0,2])
  //console.log(points.size())
  var p2 = Module.getPointAtIndex(0);
  console.log(p2);
}
