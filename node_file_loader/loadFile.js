// original example from https://stackoverflow.com/questions/57949390/how-to-do-file-inputs-via-node-js-using-emscripten
var Module = require('./inputFile.js');

if (process.argv[3] && process.argv[2] === '-f') {
   const filename = process.argv[3];
   console.log(Module);
   Module.InputFile(filename);
} else {
   console.log('Pass the file with -f flag!');
}
