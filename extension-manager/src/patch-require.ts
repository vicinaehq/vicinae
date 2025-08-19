import * as React from 'react';
import * as vicinae from '@vicinae/api';
import * as jsxRuntime from 'react/jsx-runtime';
import Module from 'module';

const requireOverrides: Record<string, any> = {
	'react': React,
	'react/jsx-runtime': jsxRuntime,
	'@vicinae/api': vicinae,
	'@raycast/api': vicinae
};

export const patchRequire = () => {
	const originalRequire = Module.prototype.require;

	// @ts-ignore
	Module.prototype.require = function(id: string) {
		return requireOverrides[id] ?? originalRequire.call(this, id);
	}
}
