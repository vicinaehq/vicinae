'use strict';

var Type = require('es-abstract/2024/Type');

var $TypeError = require('es-errors/type');

var implementation = require('./implementation');

var hasProto = [].__proto__ === Array.prototype; // eslint-disable-line no-proto

var getDunder = require('dunder-proto/get');

var getProto = function getPrototypeOf(value) {
	if (Type(value) !== 'Object') {
		throw new $TypeError('Reflect.getPrototypeOf called on non-object');
	}
	// eslint-disable-next-line no-proto
	return getDunder ? getDunder(value) : value.__proto__;
};

module.exports = function getPolyfill() {
	if (typeof Reflect === 'object' && Reflect && Reflect.getPrototypeOf) {
		return Reflect.getPrototypeOf;
	}
	if (hasProto) {
		return getProto;
	}
	return implementation;
};
