function init() {
  let id = Module.init();
  Module.addPoints(id, 2, 6);
  Module.readVec(id)
  var p2 = Module.getPointAtIndex(id, 0);
  console.log(p2);
}
