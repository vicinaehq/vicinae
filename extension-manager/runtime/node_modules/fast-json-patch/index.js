var core = require("./commonjs/core.js");
Object.assign(exports, core);

var duplex = require("./commonjs/duplex.js");
Object.assign(exports, duplex);

var helpers = require("./commonjs/helpers.js");
exports.JsonPatchError = helpers.PatchError;
exports.deepClone = helpers._deepClone;
exports.escapePathComponent = helpers.escapePathComponent;
exports.unescapePathComponent = helpers.unescapePathComponent;
