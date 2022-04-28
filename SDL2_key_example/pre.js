Module.postRun = function() {
    function doOne() {
      Module._one();
      setTimeout(doOne, 1000/60);
    }
    setTimeout(doOne, 1000/60);
  }

  function keydown(c) {
    var event = new KeyboardEvent("keydown", { 'keyCode': c, 'charCode': c, 'view': window, 'bubbles': true, 'cancelable': true });
    var prevented = !document.dispatchEvent(event);

    //send keypress if not prevented
    if (!prevented) {
      var event = new KeyboardEvent("keypress", { 'keyCode': c, 'charCode': c, 'view': window, 'bubbles': true, 'cancelable': true });
      document.dispatchEvent(event);
    }
  }

  function keyup(c) {
    var event = new KeyboardEvent("keyup", { 'keyCode': c, 'charCode': c, 'view': window, 'bubbles': true, 'cancelable': true });
    document.dispatchEvent(event);
  }