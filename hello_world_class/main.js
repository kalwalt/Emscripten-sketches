var Module = require("./hello_world.js");

Module.onRuntimeInitialized = async function(){
    // uncommentthe code below to output the Module in the console
    // console.log('Module loaded: ', Module);
    var instance = new Module.Hello(); // this will print "Hello world!!!""
    console.log(instance); // this will pint the class as an object "Hello{}""
    instance.saySomething(); // this will print "something"
}