'use strict';

var GetIntrinsic = require('get-intrinsic');
var IsCallable = require('es-abstract/2024/IsCallable');
var Type = require('es-abstract/2024/Type');
var whichBuiltinType = require('which-builtin-type');
var $TypeError = require('es-errors/type');

var $gPO = GetIntrinsic('%Object.getPrototypeOf%', true);
var $ObjectPrototype = GetIntrinsic('%Object.prototype%');

var getDunder = require('dunder-proto/get');

module.exports = function getPrototypeOf(O) {
	if (Type(O) !== 'Object') {
		throw new $TypeError('Reflect.getPrototypeOf called on non-object');
	}

	if ($gPO) {
		return $gPO(O);
	}

	if (getDunder) {
		var proto = getDunder(O);
		if (proto || proto === null) {
			return proto;
		}
	}
	var type = whichBuiltinType(O);
	if (type) {
		var intrinsic = GetIntrinsic('%' + type + '.prototype%', true);
		if (intrinsic) {
			return intrinsic;
		}
	}
	if (IsCallable(O.constructor)) {
		return O.constructor.prototype;
	}
	if (O instanceof Object) {
		return $ObjectPrototype;
	}

	/*
	 * Correctly return null for Objects created with `Object.create(null)` (shammed or native) or `{ __proto__: null}`.  Also returns null for
	 * cross-realm objects on browsers that lack `__proto__` support (like IE <11), but that's the best we can do.
	 */
	return null;
};
