var Module = require("./memory_views.js");

Module.onRuntimeInitialized = async function(){
    // uncomment the code below to output the Module in the console
    console.log('Module loaded: ', Module);
    var array = new Module.getInt8Array(); // get the array
    console.log(array); // this will print the array as Uint8Array
} 