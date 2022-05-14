var Module = require("./range_error_example.js");

Module.onRuntimeInitialized = async function(){
    // uncommentthe code below to output the Module in the console
    //console.log('Module loaded: ', Module);
    var instance;
    try {
        //instance = new Module.Pow(2, 1); // uncomment this to test the class
        instance = new Module.Pow(2,1000); // we initialize the class with a wrong exponent value
        console.log(instance); // this will pint the class as an object "Pow{}""
        var res = instance.getResult(); // this will print the result if exponent is in range
        console.log(res);
    } catch(exception){
        console.error(Module.getExceptionMessage(exception));
    } finally {
        console.log("All done!") 
    }
    
}