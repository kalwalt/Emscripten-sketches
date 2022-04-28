Module.postRun = function() {
    function doOne() {
      Module._one();
      setTimeout(doOne, 1000/60);
    }
    setTimeout(doOne, 1000/60);
  }

  function simulateKeyEvent(c) {
    var event = new KeyboardEvent("keypress", { 'keyCode': c, 'charCode': c, 'view': window, 'bubbles': true, 'cancelable': true });
    document.body.dispatchEvent(event);
  }