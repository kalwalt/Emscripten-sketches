Module.onRuntimeInitialized = async function () {
    console.log(Module);

    async function readImageData(imageUrl) {
        try {
            const response = await fetch(imageUrl);
            const blob = await response.blob();

            // Create an object URL for the image
            const objectURL = URL.createObjectURL(blob);

            // Create an image element
            const img = new Image();
            img.src = objectURL;

            // Wait for the image to load
            await new Promise((resolve) => {
                img.onload = resolve;
            });

            // Create a canvas element
            const canvas = document.createElement('canvas');
            canvas.width = img.width;
            canvas.height = img.height;

            // Get the 2D rendering context
            const ctx = canvas.getContext('2d');
            ctx.drawImage(img, 0, 0);

            // Get image data as an ImageData object
            const imageData = ctx.getImageData(0, 0, img.width, img.height);

            // Access pixel data
            const pixelData = imageData.data;

            // Example: Access the red value of the first pixel
            const firstPixelRed = pixelData[0];

            // Clean up
            URL.revokeObjectURL(objectURL);

            return { pixelData, width: img.width, height: img.height };

        } catch (error) {
            console.error('Error reading image data:', error);
            return null;
        }
    }

    function rgbaToRgb(rgbaData) {
        // Assumiamo che rgbaData sia un Uint8Array con struttura [R, G, B, A, R, G, B, A, ...]

        const rgbData = new Uint8Array(rgbaData.length / 4 * 3); // Nuovo array per i dati RGB
        let rgbIndex = 0;

        for (let i = 0; i < rgbaData.length; i += 4) {
            rgbData[rgbIndex++] = rgbaData[i];   // Copia R
            rgbData[rgbIndex++] = rgbaData[i + 1]; // Copia G
            rgbData[rgbIndex++] = rgbaData[i + 2]; // Copia B
        }

        return rgbData;
    }

    const data = await readImageData('pinball.jpg')
    console.log(data)
    const filtered = rgbaToRgb(data.pixelData);
    console.log(filtered)
    const width = 1637;
    const height = 2048;
    const res = Module.convert_to_luma(data.pixelData, 1637, 2048, true, false);
    const canvas = document.createElement('canvas');
    canvas.width = 1637;
    canvas.height = 2048;
    document.body.appendChild(canvas);
    const ctx = canvas.getContext('2d');
    const imageData = ctx.createImageData(1637, 2048);

    const alpha = 255;

    const rgbaArray = new Uint8Array(width*height*4)
    const len = res.length;
    console.log(len)
    console.log('length rgbaArray', rgbaArray.length)
    for (let i = 0, j = 0; i < len; ) {
        rgbaArray[j++] = res[i++]; // Rosso
        rgbaArray[j++] = res[i++]; // Verde
        rgbaArray[j++] = res[i++]; // Blu
        rgbaArray[j++] = alpha;       // Alpha
    }
    imageData.data.set(rgbaArray);
    ctx.putImageData(imageData, 0, 0);
    console.log("Result from convert_to_gray function: ", imageData);
}