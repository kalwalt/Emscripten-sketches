emcc worker_api_worker.cpp -o worker.js -s BUILD_AS_WORKER -s EXPORTED_FUNCTIONS=_one
emcc worker_api_main.cpp -o test.html