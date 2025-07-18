import * as React from 'react';
import * as omnicast from '@omnicast/api';
import * as jsxRuntime from 'react/jsx-runtime';
import Module from 'module';

const requireOverrides: Record<string, any> = {
	'react': React,
	'react/jsx-runtime': jsxRuntime,
	'@omnicast/api': omnicast,
	'@raycast/api': omnicast
};

export const patchRequire = () => {
	const originalRequire = Module.prototype.require;

	// @ts-ignore
	Module.prototype.require = function(id: string) {
		return requireOverrides[id] ?? originalRequire.call(this, id);
	}
}
