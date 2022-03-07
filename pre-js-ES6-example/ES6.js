; (() => {
    /* ES6 IIFE */
    'use strict'
    var scope;
    if (typeof window !== 'undefined') {
        scope = window;
    } else if (typeof global !== 'undefined') {
        scope = global;
    } else {
        scope = global;
    }

    class Message {
        constructor(msg) {
            this.msg = msg;
        }
        getMessage() {
            return this.msg;
        }
        setMessage(msg) {
            this.msg = msg;
        }
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
