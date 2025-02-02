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

            // Clean up
            URL.revokeObjectURL(objectURL);

            return { pixelData, width: img.width, height: img.height };

        } catch (error) {
            console.error('Error reading image data:', error);
            return null;
        }
    }

    function rgbaToRgb(rgbaData) {
        // We assume that RGBA data is a Uint8Array with structure [R, G, B, A, R, G, B, A, ...]

        const rgbData = new Uint8Array(rgbaData.length / 4 * 3); // New Uint8Array for RGBA data
        let rgbIndex = 0;

        for (let i = 0; i < rgbaData.length; i += 4) {
            rgbData[rgbIndex++] = rgbaData[i];   // Copy R
            rgbData[rgbIndex++] = rgbaData[i + 1]; // Copy G
            rgbData[rgbIndex++] = rgbaData[i + 2]; // Copy B
        }

        return rgbData;
    }

    const data = await readImageData('pinball.jpg')

    const width = 1637;
    const height = 2048;
    const simdType = {
        slow: 1,
        fast: 2
    }
    const res = Module.convert_to_luma(data.pixelData, 1637, 2048, true, true, simdType.slow);
    
    const canvas = document.createElement('canvas');
    canvas.width = width;
    canvas.height = height;
    document.body.appendChild(canvas);
    const ctx = canvas.getContext('2d');
    const imageData = ctx.createImageData(1637, 2048);

    const alpha = 255;

    const rgbaArray = new Uint8Array(width*height*4)
    const len = res.length;

    for (let i = 0, j = 0; i < len; i++, j += 4) {
        const v = res[i];
        rgbaArray[j + 0] = v;
        rgbaArray[j + 1] = v;
        rgbaArray[j + 2] = v;
        rgbaArray[j + 3] = alpha;
    }

    imageData.data.set(rgbaArray);
    ctx.putImageData(imageData, 0, 0);
    console.log("Result from convert_to_gray function: ", imageData);
}