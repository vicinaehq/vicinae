export * from './module/core.mjs';
export * from './module/duplex.mjs';
export {
    PatchError as JsonPatchError,
    _deepClone as deepClone,
    escapePathComponent,
    unescapePathComponent
} from './module/helpers.mjs';


/**
 * Default export for backwards compat
 */

import * as core from './module/core.mjs';
import * as duplex from './module/duplex.mjs';
import {
    PatchError as JsonPatchError,
    _deepClone as deepClone,
    escapePathComponent,
    unescapePathComponent
} from './module/helpers.mjs';

export default Object.assign({}, core, duplex, {
    JsonPatchError,
    deepClone,
    escapePathComponent,
    unescapePathComponent
});