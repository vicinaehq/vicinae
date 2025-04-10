"use strict";
var __create = Object.create;
var __defProp = Object.defineProperty;
var __getOwnPropDesc = Object.getOwnPropertyDescriptor;
var __getOwnPropNames = Object.getOwnPropertyNames;
var __getProtoOf = Object.getPrototypeOf;
var __hasOwnProp = Object.prototype.hasOwnProperty;
var __commonJS = (cb, mod) => function __require() {
  return mod || (0, cb[__getOwnPropNames(cb)[0]])((mod = { exports: {} }).exports, mod), mod.exports;
};
var __export = (target, all) => {
  for (var name in all)
    __defProp(target, name, { get: all[name], enumerable: true });
};
var __copyProps = (to, from, except, desc) => {
  if (from && typeof from === "object" || typeof from === "function") {
    for (let key of __getOwnPropNames(from))
      if (!__hasOwnProp.call(to, key) && key !== except)
        __defProp(to, key, { get: () => from[key], enumerable: !(desc = __getOwnPropDesc(from, key)) || desc.enumerable });
  }
  return to;
};
var __toESM = (mod, isNodeMode, target) => (target = mod != null ? __create(__getProtoOf(mod)) : {}, __copyProps(
  // If the importer is in node compatibility mode or this is not an ESM
  // file that has been converted to a CommonJS file using a Babel-
  // compatible transform (i.e. "__esModule" has not been set), then set
  // "default" to the CommonJS "module.exports" for node compatibility.
  isNodeMode || !mod || !mod.__esModule ? __defProp(target, "default", { value: mod, enumerable: true }) : target,
  mod
));
var __toCommonJS = (mod) => __copyProps(__defProp({}, "__esModule", { value: true }), mod);

// node_modules/fast-json-patch/commonjs/helpers.js
var require_helpers = __commonJS({
  "node_modules/fast-json-patch/commonjs/helpers.js"(exports2) {
    var __extends = exports2 && exports2.__extends || /* @__PURE__ */ function() {
      var extendStatics = function(d, b) {
        extendStatics = Object.setPrototypeOf || { __proto__: [] } instanceof Array && function(d2, b2) {
          d2.__proto__ = b2;
        } || function(d2, b2) {
          for (var p in b2) if (b2.hasOwnProperty(p)) d2[p] = b2[p];
        };
        return extendStatics(d, b);
      };
      return function(d, b) {
        extendStatics(d, b);
        function __() {
          this.constructor = d;
        }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
      };
    }();
    Object.defineProperty(exports2, "__esModule", { value: true });
    var _hasOwnProperty = Object.prototype.hasOwnProperty;
    function hasOwnProperty(obj, key) {
      return _hasOwnProperty.call(obj, key);
    }
    exports2.hasOwnProperty = hasOwnProperty;
    function _objectKeys(obj) {
      if (Array.isArray(obj)) {
        var keys_1 = new Array(obj.length);
        for (var k = 0; k < keys_1.length; k++) {
          keys_1[k] = "" + k;
        }
        return keys_1;
      }
      if (Object.keys) {
        return Object.keys(obj);
      }
      var keys = [];
      for (var i in obj) {
        if (hasOwnProperty(obj, i)) {
          keys.push(i);
        }
      }
      return keys;
    }
    exports2._objectKeys = _objectKeys;
    function _deepClone(obj) {
      switch (typeof obj) {
        case "object":
          return JSON.parse(JSON.stringify(obj));
        //Faster than ES5 clone - http://jsperf.com/deep-cloning-of-objects/5
        case "undefined":
          return null;
        //this is how JSON.stringify behaves for array items
        default:
          return obj;
      }
    }
    exports2._deepClone = _deepClone;
    function isInteger(str) {
      var i = 0;
      var len = str.length;
      var charCode;
      while (i < len) {
        charCode = str.charCodeAt(i);
        if (charCode >= 48 && charCode <= 57) {
          i++;
          continue;
        }
        return false;
      }
      return true;
    }
    exports2.isInteger = isInteger;
    function escapePathComponent(path) {
      if (path.indexOf("/") === -1 && path.indexOf("~") === -1)
        return path;
      return path.replace(/~/g, "~0").replace(/\//g, "~1");
    }
    exports2.escapePathComponent = escapePathComponent;
    function unescapePathComponent(path) {
      return path.replace(/~1/g, "/").replace(/~0/g, "~");
    }
    exports2.unescapePathComponent = unescapePathComponent;
    function _getPathRecursive(root, obj) {
      var found;
      for (var key in root) {
        if (hasOwnProperty(root, key)) {
          if (root[key] === obj) {
            return escapePathComponent(key) + "/";
          } else if (typeof root[key] === "object") {
            found = _getPathRecursive(root[key], obj);
            if (found != "") {
              return escapePathComponent(key) + "/" + found;
            }
          }
        }
      }
      return "";
    }
    exports2._getPathRecursive = _getPathRecursive;
    function getPath(root, obj) {
      if (root === obj) {
        return "/";
      }
      var path = _getPathRecursive(root, obj);
      if (path === "") {
        throw new Error("Object not found in root");
      }
      return "/" + path;
    }
    exports2.getPath = getPath;
    function hasUndefined(obj) {
      if (obj === void 0) {
        return true;
      }
      if (obj) {
        if (Array.isArray(obj)) {
          for (var i_1 = 0, len = obj.length; i_1 < len; i_1++) {
            if (hasUndefined(obj[i_1])) {
              return true;
            }
          }
        } else if (typeof obj === "object") {
          var objKeys = _objectKeys(obj);
          var objKeysLength = objKeys.length;
          for (var i = 0; i < objKeysLength; i++) {
            if (hasUndefined(obj[objKeys[i]])) {
              return true;
            }
          }
        }
      }
      return false;
    }
    exports2.hasUndefined = hasUndefined;
    function patchErrorMessageFormatter(message, args) {
      var messageParts = [message];
      for (var key in args) {
        var value = typeof args[key] === "object" ? JSON.stringify(args[key], null, 2) : args[key];
        if (typeof value !== "undefined") {
          messageParts.push(key + ": " + value);
        }
      }
      return messageParts.join("\n");
    }
    var PatchError = (
      /** @class */
      function(_super) {
        __extends(PatchError2, _super);
        function PatchError2(message, name, index, operation, tree) {
          var _newTarget = this.constructor;
          var _this = _super.call(this, patchErrorMessageFormatter(message, { name, index, operation, tree })) || this;
          _this.name = name;
          _this.index = index;
          _this.operation = operation;
          _this.tree = tree;
          Object.setPrototypeOf(_this, _newTarget.prototype);
          _this.message = patchErrorMessageFormatter(message, { name, index, operation, tree });
          return _this;
        }
        return PatchError2;
      }(Error)
    );
    exports2.PatchError = PatchError;
  }
});

// node_modules/fast-json-patch/commonjs/core.js
var require_core = __commonJS({
  "node_modules/fast-json-patch/commonjs/core.js"(exports2) {
    Object.defineProperty(exports2, "__esModule", { value: true });
    var helpers_js_1 = require_helpers();
    exports2.JsonPatchError = helpers_js_1.PatchError;
    exports2.deepClone = helpers_js_1._deepClone;
    var objOps = {
      add: function(obj, key, document) {
        obj[key] = this.value;
        return { newDocument: document };
      },
      remove: function(obj, key, document) {
        var removed = obj[key];
        delete obj[key];
        return { newDocument: document, removed };
      },
      replace: function(obj, key, document) {
        var removed = obj[key];
        obj[key] = this.value;
        return { newDocument: document, removed };
      },
      move: function(obj, key, document) {
        var removed = getValueByPointer(document, this.path);
        if (removed) {
          removed = helpers_js_1._deepClone(removed);
        }
        var originalValue = applyOperation(document, { op: "remove", path: this.from }).removed;
        applyOperation(document, { op: "add", path: this.path, value: originalValue });
        return { newDocument: document, removed };
      },
      copy: function(obj, key, document) {
        var valueToCopy = getValueByPointer(document, this.from);
        applyOperation(document, { op: "add", path: this.path, value: helpers_js_1._deepClone(valueToCopy) });
        return { newDocument: document };
      },
      test: function(obj, key, document) {
        return { newDocument: document, test: _areEquals(obj[key], this.value) };
      },
      _get: function(obj, key, document) {
        this.value = obj[key];
        return { newDocument: document };
      }
    };
    var arrOps = {
      add: function(arr, i, document) {
        if (helpers_js_1.isInteger(i)) {
          arr.splice(i, 0, this.value);
        } else {
          arr[i] = this.value;
        }
        return { newDocument: document, index: i };
      },
      remove: function(arr, i, document) {
        var removedList = arr.splice(i, 1);
        return { newDocument: document, removed: removedList[0] };
      },
      replace: function(arr, i, document) {
        var removed = arr[i];
        arr[i] = this.value;
        return { newDocument: document, removed };
      },
      move: objOps.move,
      copy: objOps.copy,
      test: objOps.test,
      _get: objOps._get
    };
    function getValueByPointer(document, pointer) {
      if (pointer == "") {
        return document;
      }
      var getOriginalDestination = { op: "_get", path: pointer };
      applyOperation(document, getOriginalDestination);
      return getOriginalDestination.value;
    }
    exports2.getValueByPointer = getValueByPointer;
    function applyOperation(document, operation, validateOperation, mutateDocument, banPrototypeModifications, index) {
      if (validateOperation === void 0) {
        validateOperation = false;
      }
      if (mutateDocument === void 0) {
        mutateDocument = true;
      }
      if (banPrototypeModifications === void 0) {
        banPrototypeModifications = true;
      }
      if (index === void 0) {
        index = 0;
      }
      if (validateOperation) {
        if (typeof validateOperation == "function") {
          validateOperation(operation, 0, document, operation.path);
        } else {
          validator(operation, 0);
        }
      }
      if (operation.path === "") {
        var returnValue = { newDocument: document };
        if (operation.op === "add") {
          returnValue.newDocument = operation.value;
          return returnValue;
        } else if (operation.op === "replace") {
          returnValue.newDocument = operation.value;
          returnValue.removed = document;
          return returnValue;
        } else if (operation.op === "move" || operation.op === "copy") {
          returnValue.newDocument = getValueByPointer(document, operation.from);
          if (operation.op === "move") {
            returnValue.removed = document;
          }
          return returnValue;
        } else if (operation.op === "test") {
          returnValue.test = _areEquals(document, operation.value);
          if (returnValue.test === false) {
            throw new exports2.JsonPatchError("Test operation failed", "TEST_OPERATION_FAILED", index, operation, document);
          }
          returnValue.newDocument = document;
          return returnValue;
        } else if (operation.op === "remove") {
          returnValue.removed = document;
          returnValue.newDocument = null;
          return returnValue;
        } else if (operation.op === "_get") {
          operation.value = document;
          return returnValue;
        } else {
          if (validateOperation) {
            throw new exports2.JsonPatchError("Operation `op` property is not one of operations defined in RFC-6902", "OPERATION_OP_INVALID", index, operation, document);
          } else {
            return returnValue;
          }
        }
      } else {
        if (!mutateDocument) {
          document = helpers_js_1._deepClone(document);
        }
        var path = operation.path || "";
        var keys = path.split("/");
        var obj = document;
        var t = 1;
        var len = keys.length;
        var existingPathFragment = void 0;
        var key = void 0;
        var validateFunction = void 0;
        if (typeof validateOperation == "function") {
          validateFunction = validateOperation;
        } else {
          validateFunction = validator;
        }
        while (true) {
          key = keys[t];
          if (key && key.indexOf("~") != -1) {
            key = helpers_js_1.unescapePathComponent(key);
          }
          if (banPrototypeModifications && (key == "__proto__" || key == "prototype" && t > 0 && keys[t - 1] == "constructor")) {
            throw new TypeError("JSON-Patch: modifying `__proto__` or `constructor/prototype` prop is banned for security reasons, if this was on purpose, please set `banPrototypeModifications` flag false and pass it to this function. More info in fast-json-patch README");
          }
          if (validateOperation) {
            if (existingPathFragment === void 0) {
              if (obj[key] === void 0) {
                existingPathFragment = keys.slice(0, t).join("/");
              } else if (t == len - 1) {
                existingPathFragment = operation.path;
              }
              if (existingPathFragment !== void 0) {
                validateFunction(operation, 0, document, existingPathFragment);
              }
            }
          }
          t++;
          if (Array.isArray(obj)) {
            if (key === "-") {
              key = obj.length;
            } else {
              if (validateOperation && !helpers_js_1.isInteger(key)) {
                throw new exports2.JsonPatchError("Expected an unsigned base-10 integer value, making the new referenced value the array element with the zero-based index", "OPERATION_PATH_ILLEGAL_ARRAY_INDEX", index, operation, document);
              } else if (helpers_js_1.isInteger(key)) {
                key = ~~key;
              }
            }
            if (t >= len) {
              if (validateOperation && operation.op === "add" && key > obj.length) {
                throw new exports2.JsonPatchError("The specified index MUST NOT be greater than the number of elements in the array", "OPERATION_VALUE_OUT_OF_BOUNDS", index, operation, document);
              }
              var returnValue = arrOps[operation.op].call(operation, obj, key, document);
              if (returnValue.test === false) {
                throw new exports2.JsonPatchError("Test operation failed", "TEST_OPERATION_FAILED", index, operation, document);
              }
              return returnValue;
            }
          } else {
            if (t >= len) {
              var returnValue = objOps[operation.op].call(operation, obj, key, document);
              if (returnValue.test === false) {
                throw new exports2.JsonPatchError("Test operation failed", "TEST_OPERATION_FAILED", index, operation, document);
              }
              return returnValue;
            }
          }
          obj = obj[key];
          if (validateOperation && t < len && (!obj || typeof obj !== "object")) {
            throw new exports2.JsonPatchError("Cannot perform operation at the desired path", "OPERATION_PATH_UNRESOLVABLE", index, operation, document);
          }
        }
      }
    }
    exports2.applyOperation = applyOperation;
    function applyPatch(document, patch, validateOperation, mutateDocument, banPrototypeModifications) {
      if (mutateDocument === void 0) {
        mutateDocument = true;
      }
      if (banPrototypeModifications === void 0) {
        banPrototypeModifications = true;
      }
      if (validateOperation) {
        if (!Array.isArray(patch)) {
          throw new exports2.JsonPatchError("Patch sequence must be an array", "SEQUENCE_NOT_AN_ARRAY");
        }
      }
      if (!mutateDocument) {
        document = helpers_js_1._deepClone(document);
      }
      var results = new Array(patch.length);
      for (var i = 0, length_1 = patch.length; i < length_1; i++) {
        results[i] = applyOperation(document, patch[i], validateOperation, true, banPrototypeModifications, i);
        document = results[i].newDocument;
      }
      results.newDocument = document;
      return results;
    }
    exports2.applyPatch = applyPatch;
    function applyReducer(document, operation, index) {
      var operationResult = applyOperation(document, operation);
      if (operationResult.test === false) {
        throw new exports2.JsonPatchError("Test operation failed", "TEST_OPERATION_FAILED", index, operation, document);
      }
      return operationResult.newDocument;
    }
    exports2.applyReducer = applyReducer;
    function validator(operation, index, document, existingPathFragment) {
      if (typeof operation !== "object" || operation === null || Array.isArray(operation)) {
        throw new exports2.JsonPatchError("Operation is not an object", "OPERATION_NOT_AN_OBJECT", index, operation, document);
      } else if (!objOps[operation.op]) {
        throw new exports2.JsonPatchError("Operation `op` property is not one of operations defined in RFC-6902", "OPERATION_OP_INVALID", index, operation, document);
      } else if (typeof operation.path !== "string") {
        throw new exports2.JsonPatchError("Operation `path` property is not a string", "OPERATION_PATH_INVALID", index, operation, document);
      } else if (operation.path.indexOf("/") !== 0 && operation.path.length > 0) {
        throw new exports2.JsonPatchError('Operation `path` property must start with "/"', "OPERATION_PATH_INVALID", index, operation, document);
      } else if ((operation.op === "move" || operation.op === "copy") && typeof operation.from !== "string") {
        throw new exports2.JsonPatchError("Operation `from` property is not present (applicable in `move` and `copy` operations)", "OPERATION_FROM_REQUIRED", index, operation, document);
      } else if ((operation.op === "add" || operation.op === "replace" || operation.op === "test") && operation.value === void 0) {
        throw new exports2.JsonPatchError("Operation `value` property is not present (applicable in `add`, `replace` and `test` operations)", "OPERATION_VALUE_REQUIRED", index, operation, document);
      } else if ((operation.op === "add" || operation.op === "replace" || operation.op === "test") && helpers_js_1.hasUndefined(operation.value)) {
        throw new exports2.JsonPatchError("Operation `value` property is not present (applicable in `add`, `replace` and `test` operations)", "OPERATION_VALUE_CANNOT_CONTAIN_UNDEFINED", index, operation, document);
      } else if (document) {
        if (operation.op == "add") {
          var pathLen = operation.path.split("/").length;
          var existingPathLen = existingPathFragment.split("/").length;
          if (pathLen !== existingPathLen + 1 && pathLen !== existingPathLen) {
            throw new exports2.JsonPatchError("Cannot perform an `add` operation at the desired path", "OPERATION_PATH_CANNOT_ADD", index, operation, document);
          }
        } else if (operation.op === "replace" || operation.op === "remove" || operation.op === "_get") {
          if (operation.path !== existingPathFragment) {
            throw new exports2.JsonPatchError("Cannot perform the operation at a path that does not exist", "OPERATION_PATH_UNRESOLVABLE", index, operation, document);
          }
        } else if (operation.op === "move" || operation.op === "copy") {
          var existingValue = { op: "_get", path: operation.from, value: void 0 };
          var error = validate([existingValue], document);
          if (error && error.name === "OPERATION_PATH_UNRESOLVABLE") {
            throw new exports2.JsonPatchError("Cannot perform the operation from a path that does not exist", "OPERATION_FROM_UNRESOLVABLE", index, operation, document);
          }
        }
      }
    }
    exports2.validator = validator;
    function validate(sequence, document, externalValidator) {
      try {
        if (!Array.isArray(sequence)) {
          throw new exports2.JsonPatchError("Patch sequence must be an array", "SEQUENCE_NOT_AN_ARRAY");
        }
        if (document) {
          applyPatch(helpers_js_1._deepClone(document), helpers_js_1._deepClone(sequence), externalValidator || true);
        } else {
          externalValidator = externalValidator || validator;
          for (var i = 0; i < sequence.length; i++) {
            externalValidator(sequence[i], i, document, void 0);
          }
        }
      } catch (e) {
        if (e instanceof exports2.JsonPatchError) {
          return e;
        } else {
          throw e;
        }
      }
    }
    exports2.validate = validate;
    function _areEquals(a, b) {
      if (a === b)
        return true;
      if (a && b && typeof a == "object" && typeof b == "object") {
        var arrA = Array.isArray(a), arrB = Array.isArray(b), i, length, key;
        if (arrA && arrB) {
          length = a.length;
          if (length != b.length)
            return false;
          for (i = length; i-- !== 0; )
            if (!_areEquals(a[i], b[i]))
              return false;
          return true;
        }
        if (arrA != arrB)
          return false;
        var keys = Object.keys(a);
        length = keys.length;
        if (length !== Object.keys(b).length)
          return false;
        for (i = length; i-- !== 0; )
          if (!b.hasOwnProperty(keys[i]))
            return false;
        for (i = length; i-- !== 0; ) {
          key = keys[i];
          if (!_areEquals(a[key], b[key]))
            return false;
        }
        return true;
      }
      return a !== a && b !== b;
    }
    exports2._areEquals = _areEquals;
  }
});

// node_modules/fast-json-patch/commonjs/duplex.js
var require_duplex = __commonJS({
  "node_modules/fast-json-patch/commonjs/duplex.js"(exports2) {
    Object.defineProperty(exports2, "__esModule", { value: true });
    var helpers_js_1 = require_helpers();
    var core_js_1 = require_core();
    var beforeDict = /* @__PURE__ */ new WeakMap();
    var Mirror = (
      /** @class */
      /* @__PURE__ */ function() {
        function Mirror2(obj) {
          this.observers = /* @__PURE__ */ new Map();
          this.obj = obj;
        }
        return Mirror2;
      }()
    );
    var ObserverInfo = (
      /** @class */
      /* @__PURE__ */ function() {
        function ObserverInfo2(callback, observer) {
          this.callback = callback;
          this.observer = observer;
        }
        return ObserverInfo2;
      }()
    );
    function getMirror(obj) {
      return beforeDict.get(obj);
    }
    function getObserverFromMirror(mirror, callback) {
      return mirror.observers.get(callback);
    }
    function removeObserverFromMirror(mirror, observer) {
      mirror.observers.delete(observer.callback);
    }
    function unobserve(root, observer) {
      observer.unobserve();
    }
    exports2.unobserve = unobserve;
    function observe(obj, callback) {
      var patches = [];
      var observer;
      var mirror = getMirror(obj);
      if (!mirror) {
        mirror = new Mirror(obj);
        beforeDict.set(obj, mirror);
      } else {
        var observerInfo = getObserverFromMirror(mirror, callback);
        observer = observerInfo && observerInfo.observer;
      }
      if (observer) {
        return observer;
      }
      observer = {};
      mirror.value = helpers_js_1._deepClone(obj);
      if (callback) {
        observer.callback = callback;
        observer.next = null;
        var dirtyCheck = function() {
          generate(observer);
        };
        var fastCheck = function() {
          clearTimeout(observer.next);
          observer.next = setTimeout(dirtyCheck);
        };
        if (typeof window !== "undefined") {
          window.addEventListener("mouseup", fastCheck);
          window.addEventListener("keyup", fastCheck);
          window.addEventListener("mousedown", fastCheck);
          window.addEventListener("keydown", fastCheck);
          window.addEventListener("change", fastCheck);
        }
      }
      observer.patches = patches;
      observer.object = obj;
      observer.unobserve = function() {
        generate(observer);
        clearTimeout(observer.next);
        removeObserverFromMirror(mirror, observer);
        if (typeof window !== "undefined") {
          window.removeEventListener("mouseup", fastCheck);
          window.removeEventListener("keyup", fastCheck);
          window.removeEventListener("mousedown", fastCheck);
          window.removeEventListener("keydown", fastCheck);
          window.removeEventListener("change", fastCheck);
        }
      };
      mirror.observers.set(callback, new ObserverInfo(callback, observer));
      return observer;
    }
    exports2.observe = observe;
    function generate(observer, invertible) {
      if (invertible === void 0) {
        invertible = false;
      }
      var mirror = beforeDict.get(observer.object);
      _generate(mirror.value, observer.object, observer.patches, "", invertible);
      if (observer.patches.length) {
        core_js_1.applyPatch(mirror.value, observer.patches);
      }
      var temp = observer.patches;
      if (temp.length > 0) {
        observer.patches = [];
        if (observer.callback) {
          observer.callback(temp);
        }
      }
      return temp;
    }
    exports2.generate = generate;
    function _generate(mirror, obj, patches, path, invertible) {
      if (obj === mirror) {
        return;
      }
      if (typeof obj.toJSON === "function") {
        obj = obj.toJSON();
      }
      var newKeys = helpers_js_1._objectKeys(obj);
      var oldKeys = helpers_js_1._objectKeys(mirror);
      var changed = false;
      var deleted = false;
      for (var t = oldKeys.length - 1; t >= 0; t--) {
        var key = oldKeys[t];
        var oldVal = mirror[key];
        if (helpers_js_1.hasOwnProperty(obj, key) && !(obj[key] === void 0 && oldVal !== void 0 && Array.isArray(obj) === false)) {
          var newVal = obj[key];
          if (typeof oldVal == "object" && oldVal != null && typeof newVal == "object" && newVal != null && Array.isArray(oldVal) === Array.isArray(newVal)) {
            _generate(oldVal, newVal, patches, path + "/" + helpers_js_1.escapePathComponent(key), invertible);
          } else {
            if (oldVal !== newVal) {
              changed = true;
              if (invertible) {
                patches.push({ op: "test", path: path + "/" + helpers_js_1.escapePathComponent(key), value: helpers_js_1._deepClone(oldVal) });
              }
              patches.push({ op: "replace", path: path + "/" + helpers_js_1.escapePathComponent(key), value: helpers_js_1._deepClone(newVal) });
            }
          }
        } else if (Array.isArray(mirror) === Array.isArray(obj)) {
          if (invertible) {
            patches.push({ op: "test", path: path + "/" + helpers_js_1.escapePathComponent(key), value: helpers_js_1._deepClone(oldVal) });
          }
          patches.push({ op: "remove", path: path + "/" + helpers_js_1.escapePathComponent(key) });
          deleted = true;
        } else {
          if (invertible) {
            patches.push({ op: "test", path, value: mirror });
          }
          patches.push({ op: "replace", path, value: obj });
          changed = true;
        }
      }
      if (!deleted && newKeys.length == oldKeys.length) {
        return;
      }
      for (var t = 0; t < newKeys.length; t++) {
        var key = newKeys[t];
        if (!helpers_js_1.hasOwnProperty(mirror, key) && obj[key] !== void 0) {
          patches.push({ op: "add", path: path + "/" + helpers_js_1.escapePathComponent(key), value: helpers_js_1._deepClone(obj[key]) });
        }
      }
    }
    function compare2(tree1, tree2, invertible) {
      if (invertible === void 0) {
        invertible = false;
      }
      var patches = [];
      _generate(tree1, tree2, patches, "", invertible);
      return patches;
    }
    exports2.compare = compare2;
  }
});

// node_modules/fast-json-patch/index.js
var require_fast_json_patch = __commonJS({
  "node_modules/fast-json-patch/index.js"(exports2) {
    var core = require_core();
    Object.assign(exports2, core);
    var duplex = require_duplex();
    Object.assign(exports2, duplex);
    var helpers = require_helpers();
    exports2.JsonPatchError = helpers.PatchError;
    exports2.deepClone = helpers._deepClone;
    exports2.escapePathComponent = helpers.escapePathComponent;
    exports2.unescapePathComponent = helpers.unescapePathComponent;
  }
});

// src/worker.tsx
var worker_exports = {};
__export(worker_exports, {
  main: () => main
});
module.exports = __toCommonJS(worker_exports);
var import_worker_threads = require("worker_threads");

// src/reconciler.ts
var import_react_reconciler = __toESM(require("react-reconciler"));
var import_node_timers = require("node:timers");
var import_constants = require("react-reconciler/constants");
var import_fast_json_patch = __toESM(require_fast_json_patch());
var ctx = {};
var isDeepEqual = (a, b) => {
  for (const key in a) {
    if (typeof b[key] === "undefined") return false;
  }
  for (const key in b) {
    if (typeof a[key] === "undefined") return false;
  }
  for (const key in a) {
    const value = a[key];
    if (typeof b[key] !== typeof value) {
      return false;
    }
    if (typeof value === "object") {
      if (Array.isArray(value) && value.length !== b[key].length) {
        console.debug(`array shortcircuit optimization`);
        return false;
      }
      if (!isDeepEqual(value, b[key])) {
        return false;
      }
      continue;
    }
    if (a[key] != b[key]) {
      return false;
    }
  }
  return true;
};
var createHostConfig = (hostCtx, callback) => {
  const hostConfig = {
    supportsMutation: true,
    supportsPersistence: false,
    supportsHydration: false,
    createInstance(type, props, root, ctx2, handle) {
      const { children, ...rest } = props;
      return {
        type,
        props: rest,
        children: []
      };
    },
    createTextInstance() {
      throw new Error(`createTextInstance is not supported`);
    },
    appendInitialChild(parent, child) {
      hostConfig.appendChild?.(parent, child);
    },
    finalizeInitialChildren(instance, type, props, root, ctx2) {
      return false;
    },
    prepareUpdate(instance, type, oldProps, newProps, root, ctx2) {
      const changes = [];
      for (const key in newProps) {
        if (key === "children") {
          continue;
        }
        const oldValue = oldProps[key];
        const newValue = newProps[key];
        if (typeof oldValue !== typeof newValue) {
          changes.push(key, newValue);
          continue;
        }
        if (typeof newValue === "object") {
          if (!isDeepEqual(newValue, oldValue)) {
            changes.push(key, newValue);
          }
          continue;
        }
        if (oldValue !== newValue) {
          changes.push(key, newValue);
        }
      }
      return changes.length > 0 ? changes : null;
    },
    shouldSetTextContent() {
      return false;
    },
    getRootHostContext(root) {
      return ctx;
    },
    getChildHostContext(parentHostContext, type, root) {
      return ctx;
    },
    getPublicInstance(instance) {
      return instance;
    },
    prepareForCommit(container) {
      return null;
    },
    resetAfterCommit() {
      callback();
      return null;
    },
    preparePortalMount(container) {
    },
    scheduleTimeout: import_node_timers.setTimeout,
    cancelTimeout: (id) => (0, import_node_timers.clearTimeout)(id),
    noTimeout: -1,
    supportsMicrotasks: false,
    scheduleMicrotask: queueMicrotask,
    isPrimaryRenderer: true,
    getCurrentEventPriority() {
      return import_constants.DefaultEventPriority;
    },
    getInstanceFromNode() {
      return null;
    },
    beforeActiveInstanceBlur() {
    },
    afterActiveInstanceBlur() {
    },
    prepareScopeUpdate(scope, instance) {
    },
    getInstanceFromScope(scope) {
      return null;
    },
    detachDeletedInstance(instance) {
    },
    // mutation methods
    appendChild(parent, child) {
      parent.children.push(child);
    },
    appendChildToContainer(container, child) {
      container.children.push(child);
    },
    insertBefore(parent, child, beforeChild) {
      const beforeIdx = parent.children.indexOf(beforeChild);
      if (beforeIdx == -1) {
        throw new Error(`insertBefore: beforeChild not found `);
      }
      if (beforeIdx == 0) parent.children.unshift(child);
      else parent.children.splice(beforeIdx - 1, 0, child);
    },
    insertInContainerBefore(container, child) {
      throw new Error(`root container can only have one child`);
    },
    removeChild(parent, child) {
      const childIdx = parent.children.indexOf(child);
      parent.children.splice(childIdx, 1);
    },
    removeChildFromContainer(container, child) {
      const childIdx = container.children.indexOf(child);
      container.children.splice(childIdx, 1);
    },
    resetTextContent() {
    },
    commitTextUpdate() {
    },
    commitMount() {
    },
    commitUpdate(instance, payload, type, prevProps, nextProps, handle) {
      let i = 0;
      while (i < payload.length) {
        const key = payload[i++];
        const value = payload[i++];
        instance.props[key] = value;
      }
    },
    hideInstance() {
    },
    hideTextInstance() {
    },
    unhideInstance() {
    },
    unhideTextInstance() {
    },
    clearContainer(container) {
      container.children = [];
    }
  };
  return hostConfig;
};
var createRenderer = (config) => {
  const { maxRendersPerSecond = 60 } = config;
  const frameTime = Math.floor(1e3 / maxRendersPerSecond);
  const container = { children: [] };
  let oldTree = [];
  let debounceTimeout = null;
  const render = () => {
    if (debounceTimeout) {
      return;
    }
    debounceTimeout = (0, import_node_timers.setTimeout)(() => {
      const start = performance.now();
      const views = [];
      const changes = (0, import_fast_json_patch.compare)(oldTree, container.children);
      const didTreeChange = changes.length > 0;
      if (didTreeChange) {
        for (let i = 0; i != container.children.length; ++i) {
          const view = container.children[i];
          views.push({ root: view, changes: [] });
        }
        config.onUpdate?.(views);
        oldTree = (0, import_fast_json_patch.deepClone)(container.children);
      }
      const end = performance.now();
      console.log(`[PERF] processed render frame in ${end - start}ms`);
      debounceTimeout = null;
    }, frameTime);
  };
  const hostConfig = createHostConfig({}, render);
  const reconciler = (0, import_react_reconciler.default)(hostConfig);
  return {
    render(element) {
      if (!container._root) {
        container._root = reconciler.createContainer(container, 0, null, false, null, "", (error) => {
          console.error("recoverable error", error);
        }, null);
      }
      reconciler.updateContainer(element, container._root, null, render);
    }
  };
};

// src/worker.tsx
var import_api = require("@omnicast/api");
var import_react = __toESM(require("react"));
var import_jsx_runtime = require("react/jsx-runtime");
var ErrorBoundary = class extends import_react.default.Component {
  constructor(props) {
    super(props);
    this.state = { error: "" };
  }
  componentDidCatch(error) {
    this.setState({ error: `${error.name}: ${error.message}` });
  }
  render() {
    const { error } = this.state;
    if (error) {
      console.error(`FUCK THE ERROR! ${error}`);
      return null;
    } else {
      return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_jsx_runtime.Fragment, { children: this.props.children });
    }
  }
};
var App = ({ component: Component }) => {
  return /* @__PURE__ */ (0, import_jsx_runtime.jsx)(ErrorBoundary, { children: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(import_api.NavigationProvider, { root: /* @__PURE__ */ (0, import_jsx_runtime.jsx)(Component, {}) }) });
};
var loadEnviron = () => {
  import_api.environment.textSize = "medium";
  import_api.environment.appearance = "dark";
  import_api.environment.canAccess = (api) => false, import_api.environment.assetsPath = "";
  import_api.environment.isDevelopment = false;
  import_api.environment.commandMode = "view";
  import_api.environment.supportPath = "/tmp";
  import_api.environment.raycastVersion = "1.0.0";
  import_api.environment.launchType = {};
};
var main = async () => {
  if (!import_worker_threads.parentPort) {
    console.error(`Unable to get workerData. Is this code running inside a NodeJS worker? Manually invoking this runtime is not supported.`);
    return;
  }
  loadEnviron();
  const module2 = await import(import_worker_threads.workerData.component);
  const Component = module2.default.default;
  process.on("uncaughtException", (error) => {
    console.error("uncaught exception:", error);
  });
  let lastRender = performance.now();
  const renderer = createRenderer({
    onInitialRender: (views) => {
      import_api.bus.emit("render", { views });
    },
    onUpdate: (views) => {
      const now = performance.now();
      const elapsed = now - lastRender;
      console.log(`[PERF] Render update (last update ${elapsed}ms ago)`);
      lastRender = now;
      import_api.bus.emit("render", { views });
    }
  });
  renderer.render(/* @__PURE__ */ (0, import_jsx_runtime.jsx)(App, { component: Component }));
};
main();
// Annotate the CommonJS export names for ESM import in node:
0 && (module.exports = {
  main
});
/*! Bundled license information:

fast-json-patch/commonjs/helpers.js:
  (*!
   * https://github.com/Starcounter-Jack/JSON-Patch
   * (c) 2017-2022 Joachim Wester
   * MIT licensed
   *)

fast-json-patch/commonjs/duplex.js:
  (*!
   * https://github.com/Starcounter-Jack/JSON-Patch
   * (c) 2017-2021 Joachim Wester
   * MIT license
   *)
*/
