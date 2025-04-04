export * from './src/core';
export * from './src/duplex';
export {
    PatchError as JsonPatchError,
    _deepClone as deepClone,
    escapePathComponent,
    unescapePathComponent
} from './src/helpers';


/**
 * Default export for backwards compat
 */

import * as core from './src/core';
import * as duplex from './src/duplex';
import {
    PatchError as JsonPatchError,
    _deepClone as deepClone,
    escapePathComponent,
    unescapePathComponent
} from './src/helpers';

export default {
    ...core,
    ...duplex,
    JsonPatchError,
    deepClone,
    escapePathComponent,
    unescapePathComponent
}