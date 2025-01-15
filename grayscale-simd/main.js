Module.onRuntimeInitialized = async function(){
    console.log(Module);
    fetch('pinball.jpg')
        .then(response => response.arrayBuffer())
        .then(buffer => {
            const byteArray = new Uint8Array(buffer);
            const res = Module.convert_to_gray(byteArray, 1637, 2048, 4, 4);
            console.log("Result from convert_to_gray function: ", res);
        });
    }