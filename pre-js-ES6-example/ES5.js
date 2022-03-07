; (function () {
    /* ES5 IIFE */
    'use strict'
    var scope;
    if (typeof window !== 'undefined') {
        scope = window;
    } else if (typeof global !== 'undefined') {
        scope = global;
    } else {
        scope = self;
    }
    var Message = function (msg) {
        this.msg = msg;
    };
    Message.prototype.getMessage = function () {
        return this.msg;
    }
    Message.prototype.setMessage = function (msg) {
        this.msg = msg;
    }

    var message = {
        UNDEFINED_MESSAGE: 0,
        MESSAGE_OK: 1
    };
    message['getMessage'] = Module['getMessage'];
    message['setMessage'] = Module['setMessage'];
    scope.message = message;
    scope.Message = Message;
})();
