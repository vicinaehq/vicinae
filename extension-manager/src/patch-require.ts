import Module from 'module';

const requireOverrides: Record<string, any> = {
	'react': require('react'),
	'react/jsx-runtime': require('react/jsx-runtime'),
	'@vicinae/api': require('@vicinae/api'),
	'@raycast/api': require('@vicinae/api/raycast')
};

export const patchRequire = () => {
	const originalRequire = Module.prototype.require;

	// @ts-ignore
	Module.prototype.require = function(id: string) {
		return requireOverrides[id] ?? originalRequire.call(this, id);
	}
}
