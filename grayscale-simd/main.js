Module.onRuntimeInitialized = async function(){
    console.log(Module);
    var bytearray = new Uint8Array([255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255]);
    var res = Module.convert_to_gray(bytearray, 2, 2, 3, 4)
    console.log("Result from convert_to_gray function: ", res);
    }