var Module = require("./simple.js");

Module.onRuntimeInitialized = async function(){
    var instance = new Module.Simple(); // this will print "Hello world!!!"
    console.log(instance); // this will pint the class as an object "Hello{}"
    instance.saySomething(); // this will print "something"
    //var message = new Module()
    console.log(message);
    var msg = new Message('hello') // we create anew instance of the Message class with msg = 'hello'
    console.log(msg);
    console.log(msg.getMessage()); // this will print "hello"
    msg.setMessage('HELLO WORLD!!');  // we set the msg to "HELLO WORLD!!"
    console.log(msg);
}